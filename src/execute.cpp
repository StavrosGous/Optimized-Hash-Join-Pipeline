#include <hardware.h>
#include <plan.h>
#include <table.h>
#include "RHMap.h"
#include "CuckooMap.h"
#include "HopscotchMap.h"
#include <iostream>
#include <cmath>
#include <string_view>
#include "unchained_ht.h"

#ifndef HASH_MAP
#define HASH_MAP 1
#endif



namespace Contest {


// using ExecuteResult = std::vector<std::vector<value_t>>;
using ExecuteResult = std::vector<column_t>;
ExecuteResult execute_impl(const Plan& plan, size_t node_idx);

// Helper for faster unaligned reads on x86; replacing small memcpy calls
// NOTE: This assumes little-endian and supports unaligned access which is valid on x86.
template <typename P>
static inline uint16_t read_u16(const P* p) {
    return *reinterpret_cast<const uint16_t*>(reinterpret_cast<const void*>(p));
}

template <typename P>
static inline int32_t read_i32(const P* p) {
    return *reinterpret_cast<const int32_t*>(reinterpret_cast<const void*>(p));
}



// HashMapSelector assertion for valid MapId values
template <int MapId, typename Key>
struct HashMapSelector {
    static_assert(MapId >= 0 && MapId <= 2, "Unsupported HASH_MAP value");
};

struct JoinAlgorithm {
    bool                                             build_left;
    ExecuteResult&                                   left;
    ExecuteResult&                                   right;
    ExecuteResult&                                   results;
    size_t                                           left_col, right_col;
    const std::vector<std::tuple<size_t, DataType>>& output_attrs;

    template <class T>
    auto run() {
        namespace views = ranges::views;
        // using hashmap_type = typename HashMapSelector<HASH_MAP, T>::type;
        
        auto& build_side = build_left ? left : right;
        auto& probe_side = build_left ? right : left;
        const size_t build_col = build_left ? left_col : right_col;
        const size_t probe_col = build_left ? right_col : left_col;

        
        ExecuteResult temp_results;
        temp_results.resize(output_attrs.size());

        auto const &temp_build_column = build_side[build_col];
        auto const &temp_probe_column = probe_side[probe_col];

        //if either side is empty, return empty result
        if (temp_build_column.buffers.empty() || temp_probe_column.buffers.empty()) {
            results.resize(output_attrs.size());
            return;
        }
        bool normal = (temp_build_column.num_rows <= temp_probe_column.num_rows);
        build_left ^= !normal;
        auto const &build_column = normal ? temp_build_column : temp_probe_column;
        auto const &probe_column = normal ? temp_probe_column : temp_build_column;

        UnchainedHashTable hash_table(build_column.num_rows);
        hash_table.build(build_column);

        //begin probing
        for (auto [probe_idx, probe_buffer] : probe_column.buffers | views::enumerate) {
            for (size_t i = 0; i < probe_buffer.data.size(); ++i) {
                value_t probe_val = probe_buffer.data[i];
                int32_t key = probe_val.int32_val.val;
                if (probe_val.int32_val.status) {
                    if (auto loc = hash_table.lookup(key); loc.size() > 0) {
                        for (auto entry_ptr : loc) {
                            uint16_t buf_idx = entry_ptr->buf_idx;
                            uint16_t idx = entry_ptr->offset;
                            size_t build_idx = buf_idx * MAX_PER_BUFFER_ENTRY + idx;
                            size_t probe_global_idx = probe_idx * MAX_PER_BUFFER_ENTRY + i;
                            size_t left_row_idx = build_left ? build_idx : probe_global_idx;
                            size_t right_row_idx = build_left ? probe_global_idx : build_idx;

                            for (auto [out_idx, attr] : output_attrs | views::enumerate) {
                                auto col_idx = std::get<0>(attr);
                                size_t target_row_idx;
                                const ExecuteResult* target_side;
                                size_t target_col_idx;

                                if (col_idx < left.size()) {
                                    target_side = &left;
                                    target_col_idx = col_idx;
                                    target_row_idx = left_row_idx;
                                } else {
                                    target_side = &right;
                                    target_col_idx = col_idx - left.size();
                                    target_row_idx = right_row_idx;
                                }

                                size_t columnar_buf_idx = target_row_idx / MAX_PER_BUFFER_ENTRY;
                                size_t columnar_row_offset = target_row_idx % MAX_PER_BUFFER_ENTRY;
                                
                                const auto& side = (*target_side)[target_col_idx];
                                const auto& buf = side.buffers[columnar_buf_idx];
                                value_t val = buf.data[columnar_row_offset];

                                auto& res_col = temp_results[out_idx];
                                if (res_col.buffers.empty() || (res_col.buffers.back().data.size() == MAX_PER_BUFFER_ENTRY)) {
                                    res_col.buffers.emplace_back();
                                }
                                res_col.buffers.back().data.push_back(std::move(val));
                            }
                        }
                    }
                }
            }
        }
        results = std::move(temp_results);
        auto num_rows = results[0].buffers.empty() ? 0 : (results[0].buffers.size() - 1) * MAX_PER_BUFFER_ENTRY + results[0].buffers.back().data.size();
        for (int i = 0; i < results.size(); ++i) {
            results[i].num_rows = num_rows;
        }
    }
};

ExecuteResult execute_hash_join(const Plan&          plan,
    const JoinNode&                                  join,
    const std::vector<std::tuple<size_t, DataType>>& output_attrs) {
    auto                           left_idx    = join.left;
    auto                           right_idx   = join.right;
    auto&                          left_node   = plan.nodes[left_idx];
    auto&                          right_node  = plan.nodes[right_idx];
    auto&                          left_attr_vec  = left_node.output_attrs;
    auto&                          right_attr_vec = right_node.output_attrs;
    auto                           left        = execute_impl(plan, left_idx);
    auto                           right       = execute_impl(plan, right_idx);
    std::vector<column_t> results;
    // Heuristic reservation: helps reduce reallocations for result storage
    results.reserve(std::min(left.size() + right.size(), static_cast<size_t>(left.size() * 2)));

    JoinAlgorithm join_algorithm{.build_left = join.build_left,
        .left                                = left,
        .right                               = right,
        .results                             = results,
        .left_col                            = join.left_attr,
        .right_col                           = join.right_attr,
        .output_attrs                        = output_attrs};
    join_algorithm.run<int32_t>();
    return std::move(results); // Use std::move to avoid copying
}



// fetch value from columns
void build_column_inserter(const size_t table_id, const size_t col_id, const Column& column, DataType data_type, column_t& new_column) {
    namespace views = ranges::views;
    auto& pages = column.pages;
    std::vector<buffer_t> buffers;
    buffer_t curbuf;
    switch (data_type) {
        case DataType::INT32: {
            for (auto& page : pages) {

                const uint8_t* page_data = reinterpret_cast<const uint8_t*>(page->data);

                uint16_t nr = read_u16(page_data);

                // Prepare bitmap
                size_t bitmap_size = (nr + 7) / 8;
                const uint8_t* bitmap_data = reinterpret_cast<const uint8_t*>(page_data) + PAGE_SIZE - bitmap_size;
                
                size_t cur_nv_row = 0;

                for (size_t i = 0; i < nr; ++i) {
                    // Calculate bitmap position
                    size_t byte_idx = i / 8;
                    size_t bit_pos = i % 8;

                    value_t wrapper_val;

                    if (bitmap_data[byte_idx] & (1 << bit_pos)) [[likely]] {
                        wrapper_val.int32_val.val = read_i32(page_data + 4 + 4 * (cur_nv_row++));
                        wrapper_val.int32_val.status = 1;
                    }
                    else {
                        wrapper_val.int32_val.status = 0;
                    }
                    curbuf.data.push_back(std::move(wrapper_val));
                    if (curbuf.data.size() == MAX_PER_BUFFER_ENTRY) {
                        buffers.push_back(std::move(curbuf));
                        curbuf.data.clear();
                    }
                }
            }
            break;
        }
        case DataType::VARCHAR: {
            
            for (const auto& [idx, page] : pages | views::enumerate) {
                const uint8_t* page_data = reinterpret_cast<const uint8_t*>(page->data);
                uint16_t nr = read_u16(page_data);

                if (nr == 0xffff) {
                    value_t val;
                    val.str_val.table_id = table_id;
                    val.str_val.col_id = col_id;
                    val.str_val.page_id = idx;
                    val.str_val.offset = 4;
                    curbuf.data.push_back(std::move(val));
                    if (curbuf.data.size() == MAX_PER_BUFFER_ENTRY) {
                        buffers.push_back(std::move(curbuf));
                        curbuf.data.clear();
                    }
                    continue;
                }
                else if (nr == 0xfffe) {
                    // continue to next page
                    continue;
                }
                // Prepare bitmap

                size_t bitmap_size = (nr + 7) / 8;
                const uint8_t* bitmap_data = reinterpret_cast<const uint8_t*>(page->data) + PAGE_SIZE - bitmap_size;
                
                size_t cur_nv_row = 0;
                for (size_t i = 0; i < nr; ++i) {
                    size_t byte_idx = i / 8;
                    size_t bit_pos = i % 8;
                    value_t val;
                    val.str_val.table_id = table_id;
                    val.str_val.col_id = col_id;
                    val.str_val.page_id = idx;
                    val.str_val.offset = (bitmap_data[byte_idx] & (1 << bit_pos)) ? 4 + 2 * (cur_nv_row++) : 0xFFFF;
                    curbuf.data.push_back(std::move(val));
                    if (curbuf.data.size() == MAX_PER_BUFFER_ENTRY) {
                        buffers.push_back(std::move(curbuf));
                        curbuf.data.clear();
                    }
                }
            }
            break;
        }
    }
    if (!curbuf.data.empty()) {
        buffers.push_back(std::move(curbuf));
    }
    new_column.num_rows = (buffers.empty()) ? 0 : (buffers.size() - 1) * MAX_PER_BUFFER_ENTRY + buffers.back().data.size();
    new_column.buffers = std::move(buffers);
}

ExecuteResult execute_scan(const Plan&               plan,
    const ScanNode&                                  scan,
    const std::vector<std::tuple<size_t, DataType>>& output_attrs) {
    auto                           table_id = scan.base_table_id;
    auto&                          input    = plan.inputs[table_id];
    ExecuteResult                results(output_attrs.size());
    // Reserve inner vector capacity once, before the first column inserter is called
    for (const auto& [idx, attr] : output_attrs | ranges::views::enumerate) {
        auto col_idx = std::get<0>(attr);
        auto& column = input.columns[col_idx];
        DataType data_type = column.type;
        results[idx] = column_t(data_type);
        build_column_inserter(table_id, col_idx, column, data_type, results[idx]);
    }
    return std::move(results);
}

ExecuteResult execute_impl(const Plan& plan, size_t node_idx) {
    auto& node = plan.nodes[node_idx];
    auto ret = (std::visit(
        [&](const auto& value) {
            using T = std::decay_t<decltype(value)>;
            if constexpr (std::is_same_v<T, JoinNode>) {
                auto ret  = execute_hash_join(plan, value, node.output_attrs);
                return std::move(ret);
            } else {
                auto ret = execute_scan(plan, value, node.output_attrs);
                return std::move(ret);
            }
        },
        node.data));
    return std::move(ret);
}

ColumnarTable materialize_columnar_table(const Plan& plan, const ExecuteResult& result, const std::vector<DataType>& attr_vec) {
    ColumnarTable table;
    table.num_rows = result.empty() ? 0 : result[0].num_rows;
    // std::cout << table.num_rows << std::endl; // ONLY PRINTS 0 (PROBLEM?)
    // std::cout << "before" << std::endl;
    for (size_t col_idx = 0; col_idx < attr_vec.size(); ++col_idx) {
        Column column(attr_vec[col_idx]);
        ColumnInserter<std::string> inserter_str{column};
        ColumnInserter<int32_t> inserter_int32{column};
        column_t col = result[col_idx];
        if (col.buffers.empty()) {
            for (size_t row_idx = 0; row_idx < table.num_rows; ++row_idx) {
                if (attr_vec[col_idx] == DataType::INT32) {
                    inserter_int32.insert_null();
                } else if (attr_vec[col_idx] == DataType::VARCHAR) {
                    inserter_str.insert_null();
                }
            }
        } else {
            for (size_t row_idx = 0; row_idx < table.num_rows; ++row_idx) {
                value_t val = col.get_value(row_idx);
                if (val.str_val.offset == 0xFFFF || !val.int32_val.status) [[unlikely]] {
                    if (attr_vec[col_idx] == DataType::INT32) {
                        inserter_int32.insert_null();
                    } else if (attr_vec[col_idx] == DataType::VARCHAR) {
                        inserter_str.insert_null();
                    }
                } else {
                    if (attr_vec[col_idx] == DataType::VARCHAR) {
                        // fetch string from corresponding page
                        auto& table = plan.inputs[val.str_val.table_id];
                        auto& column = table.columns[val.str_val.col_id];
                        Page* page = column.pages[val.str_val.page_id];
                        const uint8_t* page_data = reinterpret_cast<const uint8_t*>(page->data);
                        size_t offset = val.str_val.offset;
                        std::string str;
                        std::string_view sview{nullptr, 0};
                        uint16_t nr = read_u16(page_data);
                        uint16_t page_id = val.str_val.page_id;
                        bool is_long = false;
                        if (nr == 0xffff) [[unlikely]] {
                            uint16_t nchars = read_u16(page_data + 2);
                            str.assign(reinterpret_cast<const char*>(page_data + 4), nchars);
                            page_id++;
                            page = column.pages[page_id];
                            page_data = reinterpret_cast<const uint8_t*>(page->data);
                            nr = read_u16(page_data);
                            is_long = true;
                            while (nr == 0xfffe) [[likely]] {
                                nchars = read_u16(page_data + 2);
                                str.append(reinterpret_cast<const char*>(page_data + 4), nchars);
                                page_id++;
                                page = column.pages[page_id];
                                page_data = reinterpret_cast<const uint8_t*>(page->data);
                                nr = read_u16(page_data);
                            }
                        }
                        if (!is_long) [[likely]]{
                            uint16_t end_offset_pos = offset;
                            uint16_t end_offset;
                            uint16_t start_offset;
                            uint16_t base_offset = 4 + nr * 2;
                            if (end_offset_pos > 4) {
                                end_offset = read_u16(page_data + end_offset_pos) + base_offset;
                                start_offset = read_u16(page_data + end_offset_pos - 2) + base_offset;
                            } else {
                                start_offset = base_offset;
                                end_offset = read_u16(page_data + 4) + base_offset;
                            }
                            sview = std::string_view(reinterpret_cast<const char*>(page_data + start_offset), end_offset - start_offset);
                        }
                        if (is_long) {
                            inserter_str.insert(str);
                        } else {
                            inserter_str.insert(sview);
                        }
                    } else {
                        inserter_int32.insert(val.int32_val.val);
                    }
                }
            }
        }
        inserter_int32.finalize();
        inserter_str.finalize();
        table.columns.push_back(std::move(column));
    }
    return table;
}

ColumnarTable execute(const Plan& plan, [[maybe_unused]] void* context) {
    namespace views = ranges::views;
    auto ret        = execute_impl(plan, plan.root);
    std::vector<DataType> ret_attr_vec;
    ret_attr_vec.reserve(plan.nodes[plan.root].output_attrs.size());
    for (const auto& attr : plan.nodes[plan.root].output_attrs) {
        ret_attr_vec.push_back(std::move(std::get<1>(attr)));
    }
    ColumnarTable table = materialize_columnar_table(plan, ret, ret_attr_vec);
    return table;
}

void* build_context() {
    return nullptr;
}

void destroy_context([[maybe_unused]] void* context) {}

} // namespace Contest