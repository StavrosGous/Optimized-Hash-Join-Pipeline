#include <hardware.h>
#include <plan.h>
#include <table.h>
#include "RHMap.h"
#include "CuckooMap.h"
#include "HopscotchMap.h"
#include <iostream>
#include <cmath>
#include <string_view>
// #include "bitmap.h"

#ifndef HASH_MAP
#define HASH_MAP 1
#endif


#define MAX_PER_BUFFER_ENTRY (PAGE_SIZE / sizeof(int64_t))


namespace Contest {


struct string_struct {
    uint16_t table_id;
    uint16_t col_id;
    uint16_t page_id;
    uint16_t offset;
};

struct int32_wrapper {
    int32_t val;
    int32_t status;
    inline bool notNull() {
        return status & 1;
    }
};

union value_t {
    int32_wrapper int32_val;
    string_struct str_val;
};

struct buffer_t {
    std::vector<value_t> data; // 64-bit entries of type string_struct/int32_wrapper
};


struct column_t {
    std::vector<buffer_t> buffers;
    size_t                num_rows;
    DataType              type;

    column_t() : num_rows(0), type(DataType::INT32) {}
    column_t(const DataType& dt) : num_rows(0), type(dt) {}
};

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

// Specializations for different hashmap implementations
template <typename Key>
struct HashMapSelector<0, Key> {
    using type = RHMap<Key, std::vector<size_t>>;
};

template <typename Key>
struct HashMapSelector<1, Key> {
    using type = HopscotchMap<Key, std::vector<size_t>>;
};

template <typename Key>
struct HashMapSelector<2, Key> {
    using type = CuckooMap<Key, std::vector<size_t>>;
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
        using hashmap_type = typename HashMapSelector<HASH_MAP, T>::type;
        auto& build_side = build_left ? left : right;
        auto& probe_side = build_left ? right : left;
        const size_t build_col = build_left ? left_col : right_col;
        const size_t probe_col = build_left ? right_col : left_col;

        hashmap_type hash_table(build_side.size() * 1.6);
        std::cout << "Starting build phase..." << std::endl;
        ExecuteResult temp_results;
        temp_results.resize(output_attrs.size());

        std::cout << "trying to fetch build column with idx " << build_col << " build side size: " << build_side.size() << std::endl;
        const auto& build_column = build_side[build_col];

        std::cout << "trying to fetch probe column with idx " << probe_col << " probe side size: " << probe_side.size() << std::endl;
        const auto& probe_column = probe_side[probe_col];

        // Fail-safe: if build or probe column is empty, return empty columns
        if (build_column.buffers.empty() || probe_column.buffers.empty()) {
            std::cout << "[HashJoin] Build or probe column is empty. Returning empty join result." << std::endl;
            results.clear();
            results.resize(output_attrs.size());
            return;
        }

        std::cout << "build column buffers size: " << build_column.buffers.size() << std::endl;
        for (auto [idx, buffer] : build_column.buffers | views::enumerate) {
            std::cout << "Processing build buffer index: " << idx << std::endl;
            for (value_t val : buffer.data) {
                int32_t key = val.int32_val.val;
                std::cout << "Building key: " << key << " at buffer index: " << idx << std::endl;
                if (val.int32_val.status) {
                    if (auto loc = hash_table.find(key); loc == hash_table.end()) {
                        hash_table.emplace(key, std::vector<size_t>{});
                    } else {
                        auto row_idx = idx * MAX_PER_BUFFER_ENTRY + (&val - (&buffer.data[0])) / sizeof(value_t);
                        std::cout << "Row idx:" << row_idx << std::endl;
                        loc->push_back(row_idx);
                    }
                }
            }
        }

        // For each probe row, check for matches and build output buffers row-wise
        for (auto [probe_buf_idx, probe_buffer] : probe_column.buffers | views::enumerate) {
            size_t probe_row_base = probe_buf_idx * MAX_PER_BUFFER_ENTRY;
            for (size_t probe_row_offset = 0; probe_row_offset < probe_buffer.data.size(); ++probe_row_offset) {
                const value_t& probe_val = probe_buffer.data[probe_row_offset];
                int32_t key = probe_val.int32_val.val;
                if (probe_val.int32_val.status) {
                    auto loc = hash_table.find(key);
                    if (loc != hash_table.end()) {
                        for (auto build_idx : *loc) {
                            // For each output column, collect the value for this join result row
                            std::vector<value_t> output_row;
                            output_row.reserve(output_attrs.size());
                            for (auto [out_idx, attr] : output_attrs | views::enumerate) {
                            size_t col_idx = std::get<0>(attr);
                            DataType dtype = std::get<1>(attr);
                            const ExecuteResult& src_side = (build_left ? left : right);
                            const ExecuteResult& other_side = (build_left ? right : left);
                            size_t src_buf_idx, src_row_offset;
                            if (col_idx < src_side.size()) {
                                // Value comes from build side
                                src_buf_idx = build_idx / MAX_PER_BUFFER_ENTRY;
                                src_row_offset = build_idx % MAX_PER_BUFFER_ENTRY;
                                output_row.push_back(src_side[col_idx].buffers[src_buf_idx].data[src_row_offset]);
                            } else {
                                // Value comes from probe side
                                size_t probe_col_idx = col_idx - src_side.size();
                                src_buf_idx = probe_buf_idx;
                                src_row_offset = probe_row_offset;
                                output_row.push_back(other_side[probe_col_idx].buffers[src_buf_idx].data[src_row_offset]);
                            }
                            }
                            // Now, push each value to the corresponding column buffer
                            for (size_t col = 0; col < output_row.size(); ++col) {
                            if (temp_results[col].buffers.empty() || temp_results[col].buffers.back().data.size() == MAX_PER_BUFFER_ENTRY) {
                                temp_results[col].buffers.emplace_back();
                            }
                            temp_results[col].buffers.back().data.push_back(output_row[col]);
                            temp_results[col].num_rows++;
                            }
                        }
                    }
                }
            }
        }
        results = std::move(temp_results);
        // // Added std::move to avoid unnecessary copies
        // for (auto& probe_record : probe_side) {
        //     const auto& key = probe_record[probe_col];
        //     if (!key.is_null) {
        //         if (auto loc = hash_table.find(key.int32_val); loc != hash_table.end()) {
        //             for (auto build_idx : *loc) {
        //                 const auto& build_record = build_side[build_idx];
        //                 const auto* left_record_ptr = build_left ? &build_record : &probe_record;
        //                 const auto* right_record_ptr = build_left ? &probe_record : &build_record;
        //                 std::vector<value_t> new_record;
        //                 new_record.reserve(output_attrs.size());
        //                 for (auto [col_idx, _] : output_attrs) {
        //                     if (col_idx < left_record_ptr->size()) {
        //                         new_record.emplace_back(std::move((*left_record_ptr)[col_idx]));
        //                     } else {
        //                         new_record.emplace_back(std::move((*right_record_ptr)[col_idx - left_record_ptr->size()]));
        //                     }
        //                 }
        //                 results.emplace_back(std::move(new_record)); // Use std::move to avoid copying
        //             }
        //         }
        //     }
        // }
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
    switch (data_type) {
        case DataType::INT32: {
            // locate the page that contains the requested row
            auto& pages = column.pages;
            std::vector<buffer_t> buffers;
            // JONI implementation commented out
            // size_t cur_page = 0;
            // while (cur_page < pages.size()) {
            //     buffer_t curbuf;
            //     size_t rem = MAX_PER_BUFFER_ENTRY;
            //     auto& page = pages[cur_page];
            //     const uint8_t* page_data = reinterpret_cast<const uint8_t*>(page->data);
            //     uint16_t nr = read_u16(page_data);
            //     size_t bitmap_size = (nr + 7) / 8;
            //     const uint8_t* bitmap_data = reinterpret_cast<const uint8_t*>(page_data) + PAGE_SIZE - bitmap_size;
            //     size_t base_offset = 4;
            //     //Fill buf with the whole page
            //     while (rem > nr && cur_page < pages.size()) {
            //         for (size_t i = 0; i < nr; ++i) {
            //             value_t val;
            //             size_t byte_idx = i / 8;
            //             size_t bit_pos = i % 8;
            //             if (bitmap_data[byte_idx] & (1 << bit_pos)) [[likely]] {
            //                 val.int32_val = {read_i32(page_data + base_offset + 4 * nr), 1};
            //                 curbuf.data.push_back(val);
            //                 continue;
            //             }
            //         }
            //         rem -= nr;
            //         page_data = reinterpret_cast<const uint8_t*>(pages[++cur_page]->data);
            //         nr = read_u16(reinterpret_cast<const uint8_t*>(page->data));
            //     }
            //     //Fill remainder of buffer with as much is needed from the page
            //     for (size_t i = 0; i < rem; ++i) {
                        
                        
                    
            //         buffers.push_back(curbuf); // Bazoume to curbuf sto vector
            //         continue;
            //     }
            //     buffers.push_back(curbuf);
            //     curbuf.values.clear();
            //     curbuf.bitmap.clear();
            // }
            size_t cur_row = 0;
            buffer_t curbuf;
            for (auto& page : pages) {
                const uint8_t* page_data = reinterpret_cast<const uint8_t*>(page->data);
                uint16_t nr = read_u16(page_data);
                size_t bitmap_size = (nr + 7) / 8;
                const uint8_t* bitmap_data = reinterpret_cast<const uint8_t*>(page_data) + PAGE_SIZE - bitmap_size;
                size_t base_offset = 4;
                std::cout << "rows in page: " << nr << std::endl;
                for (size_t i = 0; i < nr; ++i, ++cur_row) {
                    if (curbuf.data.size() == MAX_PER_BUFFER_ENTRY) {
                        buffers.push_back(curbuf);
                        curbuf.data.clear();
                    }
                    size_t byte_idx = i / 8;
                    size_t bit_pos = i % 8;
                    value_t val;
                    bool is_not_null = bitmap_data[byte_idx] & (1 << bit_pos);
                    if (!is_not_null) {
                        std::cout << "value is null" << std::endl;
                    }
                    if (bitmap_data[byte_idx] & (1 << bit_pos)) [[likely]] {
                        val.int32_val.val = read_i32(page_data + base_offset + 4 * i);
                        val.int32_val.status = 1;
                        curbuf.data.push_back(val);
                        continue;
                    }
                    val.int32_val.status = 0;
                    curbuf.data.push_back(val);
                }
            }
            // Patch: push the last buffer if it has any data (handles single-row columns)
            if (!curbuf.data.empty()) {
                buffers.push_back(curbuf);
            }
            std::cout << "Total INT32 rows processed for column " << col_id << " of table " << table_id << ": " << cur_row << " and filled " << curbuf.data.size() << " entries." << std::endl;
            new_column.buffers = std::move(buffers);
            break;
        }
        case DataType::VARCHAR: {
            auto& pages = column.pages;
            size_t cur_row = 0;
            std::vector<buffer_t> buffers;
            buffer_t curbuf;
            for (const auto& [idx, page] : pages | views::enumerate) {
                const uint8_t* page_data = reinterpret_cast<const uint8_t*>(page->data);
                uint16_t nr = read_u16(page_data);

                // uint16_t nchars;
                // memcpy(&nchars, &page_data[2], sizeof(uint16_t));

                size_t bitmap_size = (nr + 7) / 8;
                const uint8_t* bitmap_data = reinterpret_cast<const uint8_t*>(page->data) + PAGE_SIZE - bitmap_size;

                if (nr == 0xffff) {
                    value_t val;
                    val.str_val.table_id = table_id;
                    val.str_val.col_id = col_id;
                    val.str_val.page_id = idx;
                    val.str_val.offset = 4;
                    curbuf.data.push_back(val);
                    continue;
                }
                else if (nr == 0xfffe) {
                    // continue to next page
                    continue;
                }
                uint16_t end_offset_idx = 4;
                std::cout << "rows in page: " << nr << std::endl;

                for (size_t i = 0; i < nr; ++i, ++cur_row) {
                    if (curbuf.data.size() == MAX_PER_BUFFER_ENTRY) {
                        buffers.push_back(curbuf);
                        curbuf.data.clear();
                    }
                    size_t byte_idx = i / 8;
                    size_t bit_pos = i % 8;
                    value_t val;
                    bool is_not_null = bitmap_data[byte_idx] & (1 << bit_pos);
                    if (!is_not_null) {
                        std::cout << "value is null" << std::endl;
                    }
                    if (bitmap_data[byte_idx] & (1 << bit_pos)) {
                        val.str_val.table_id = table_id;
                        val.str_val.col_id = col_id;
                        val.str_val.page_id = idx;
                        val.str_val.offset = end_offset_idx;
                        curbuf.data.push_back(val);
                        end_offset_idx += 2;
                        continue;
                    }
                    val.str_val.table_id = table_id;
                    val.str_val.col_id = col_id;
                    val.str_val.page_id = idx;
                    val.str_val.offset = 0xFFFF; // null indicator
                    curbuf.data.push_back(val);
                }
            }
            std::cout << "Total VARCHAR rows processed for column " << col_id << " of table " << table_id << ": " << cur_row << " and filled " << curbuf.data.size() << " entries." << std::endl;
            new_column.buffers = std::move(buffers);
            break;
        }
    }
}

ExecuteResult execute_scan(const Plan&               plan,
    const ScanNode&                                  scan,
    const std::vector<std::tuple<size_t, DataType>>& output_attrs) {
    auto                           table_id = scan.base_table_id;
    auto&                          input    = plan.inputs[table_id];
    ExecuteResult                results(input.columns.size());
    // Reserve inner vector capacity once, before the first column inserter is called
    for (const auto& [col_idx, data_type] : output_attrs) { 
        auto& column = input.columns[col_idx];
        results[col_idx] = column_t(data_type);
        build_column_inserter(table_id, col_idx, column, data_type, results[col_idx]);
        // print results[col_idx] size
        std::cout << "Column " << col_idx << " of table " << table_id << " has " << results[col_idx].buffers.size() << " buffers." << std::endl;
    }
    return std::move(results);
}

ExecuteResult execute_impl(const Plan& plan, size_t node_idx) {
    auto& node = plan.nodes[node_idx];
    return std::move(std::visit(
        [&](const auto& value) {
            using T = std::decay_t<decltype(value)>;
            if constexpr (std::is_same_v<T, JoinNode>) {
                return execute_hash_join(plan, value, node.output_attrs);
            } else {
                return execute_scan(plan, value, node.output_attrs);
            }
        },
        node.data));
}

ColumnarTable materialize_columnar_table(const Plan& plan, const ExecuteResult& result, const std::vector<DataType>& attr_vec) {
    ColumnarTable table;
    table.num_rows = result.size();
    for (size_t col_idx = 0; col_idx < attr_vec.size(); ++col_idx) {
        Column column(attr_vec[col_idx]);
        ColumnInserter<std::string> inserter_str{column};
        ColumnInserter<int32_t> inserter_int32{column};
        for (size_t row_idx = 0; row_idx < result.size(); ++row_idx) {
            column_t col = result[col_idx];
            value_t val = col.buffers[row_idx / MAX_PER_BUFFER_ENTRY].data[row_idx % MAX_PER_BUFFER_ENTRY];
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
                    // column is INT32
                    inserter_int32.insert(val.int32_val.val);
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
        ret_attr_vec.push_back(std::get<1>(attr));
    }
    return materialize_columnar_table(plan, ret, ret_attr_vec);
}

void* build_context() {
    return nullptr;
}

void destroy_context([[maybe_unused]] void* context) {}

} // namespace Contest