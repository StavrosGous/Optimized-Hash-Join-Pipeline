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


#define MAX_PER_BUFFER_INT32 (PAGE_SIZE / sizeof(int32_t))
#define MAX_PER_BUFFER_STRING (PAGE_SIZE / sizeof(string_struct))

namespace Contest {


struct string_struct {
    uint16_t table_id;
    uint16_t col_id;
    uint16_t page_id;
    uint16_t offset;
};

union value_t {
    int32_wrapper int32_val;
    string_struct str_val;
};

struct buffer_t {
    std::vector<uint64_t> data[PAGE_SIZE]; // 64-byte entries of type string_struct/int32_wrapper
};

struct int32_wrapper {
    int32_t val;
    int32_t status;
    inline bool notNull() {
        return status & 1;
    }
};

struct column_t {
    std::vector<buffer_t> buffers;
    size_t                num_rows;
    DataType              type;

    column_t() : num_rows(0), type(DataType::INT32) {}
    column_t(const DataType& dt) : num_rows(0), type(dt) {}
};

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


// using ExecuteResult = std::vector<std::vector<value_t>>;
using ExecuteResult = std::vector<column_t>;

ExecuteResult execute_impl(const Plan& plan, size_t node_idx);

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
        using hashmap_type = typename HashMapSelector<HASH_MAP, T>::type; // select map based on compilation flag HASH_MAP
        // choose build and probe sides uniformly to avoid duplicate branches
        auto& build_side = build_left ? left : right;
        auto& probe_side = build_left ? right : left;
        const size_t build_col = build_left ? left_col : right_col;
        const size_t probe_col = build_left ? right_col : left_col;

        hashmap_type hash_table(build_side.size() * 1.6);
        for (auto&& [idx, record] : build_side | views::enumerate) {
            const auto& key = record[build_col];
            const auto& key_value = key.int32_val;
            if (!key.is_null) {
                if (auto loc = hash_table.find(key_value); loc == hash_table.end()) {
                    hash_table.emplace(key_value, std::vector<size_t>(1, idx));
                } else {
                    loc->push_back(idx);
                }
            }
        }

        // Added std::move to avoid unnecessary copies
        for (auto& probe_record : probe_side) {
            const auto& key = probe_record[probe_col];
            if (!key.is_null) {
                if (auto loc = hash_table.find(key.int32_val); loc != hash_table.end()) {
                    for (auto build_idx : *loc) {
                        const auto& build_record = build_side[build_idx];
                        const auto* left_record_ptr = build_left ? &build_record : &probe_record;
                        const auto* right_record_ptr = build_left ? &probe_record : &build_record;
                        std::vector<value_t> new_record;
                        new_record.reserve(output_attrs.size());
                        for (auto [col_idx, _] : output_attrs) {
                            if (col_idx < left_record_ptr->size()) {
                                new_record.emplace_back(std::move((*left_record_ptr)[col_idx]));
                            } else {
                                new_record.emplace_back(std::move((*right_record_ptr)[col_idx - left_record_ptr->size()]));
                            }
                        }
                        results.emplace_back(std::move(new_record)); // Use std::move to avoid copying
                    }
                }
            }
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
    switch (data_type) {
        case DataType::INT32: {
            // locate the page that contains the requested row
            auto& pages = column.pages;
            std::vector<buffer_t> buffers;
            size_t cur_page = 0;
            while (cur_page < pages.size()) {
                buffer_t curbuf;
                size_t rem = MAX_PER_BUFFER_INT32;
                auto& page = pages[cur_page];
                const uint8_t* page_data = reinterpret_cast<const uint8_t*>(page->data);
                uint16_t nr = read_u16(page_data);
                size_t bitmap_size = (nr + 7) / 8;
                const uint8_t* bitmap_data = reinterpret_cast<const uint8_t*>(page_data) + PAGE_SIZE - bitmap_size;
                size_t base_offset = 4;
                //Fill buf with the whole page
                while (rem > nr && cur_page < pages.size()) {
                    for (size_t i = 0; i < nr; ++i) {
                        value_t val;
                        size_t byte_idx = i / 8;
                        size_t bit_pos = i % 8;
                        if (bitmap_data[byte_idx] & (1 << bit_pos)) [[likely]] {
                            val.int32_val = {read_i32(page_data + base_offset + 4 * nr), 1};
                            curbuf.data.push_back(val);
                            continue;
                        }
                    }
                    rem -= nr;
                    page_data = reinterpret_cast<const uint8_t*>(pages[++cur_page]->data);
                    nr = read_u16(reinterpret_cast<const uint8_t*>(page->data));
                }
                //Fill remainder of buffer with as much is needed from the page
                for (size_t i = 0; i < rem; ++i) {
                        
                        
                    
                    buffers.push_back(curbuf); // Bazoume to curbuf sto vector
                    continue;
                }
                buffers.push_back(curbuf);
                curbuf.values.clear();
                curbuf.bitmap.clear();
            }
            int cur_row = 0;
            for (auto& page : pages) {
                const uint8_t* page_data = reinterpret_cast<const uint8_t*>(page->data);
                uint16_t nr = read_u16(page_data);
                size_t bitmap_size = (nr + 7) / 8;
                const uint8_t* bitmap_data = reinterpret_cast<const uint8_t*>(page_data) + PAGE_SIZE - bitmap_size;
                size_t base_offset = 4;
                for (size_t cur_byte_idx = 0, j = 0; j < nr; ++j, ++cur_row) {
                    size_t byte_idx = j / 8;
                    size_t bit_pos = j % 8;
                    value_t val;
                    if (bitmap_data[byte_idx] & (1 << bit_pos)) [[likely]] {
                        val.int32_val = read_i32(page_data + base_offset + cur_byte_idx);
                        results[cur_row].push_back(val);
                        cur_byte_idx += 4;
                        continue;
                    }
                    results[cur_row].push_back(val);
                }
            }
            break;
        }
        case DataType::VARCHAR: {
            auto& pages = column.pages;
            size_t cur_row = 0;
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
                    results[cur_row++].push_back(val);
                    continue;
                }
                else if (nr == 0xfffe) {
                    // continue to next page
                    continue;
                }
                uint16_t end_offset_idx = 4;
                for (size_t j = 0; j < nr; ++j, ++cur_row) {
                    size_t byte_idx = j / 8;
                    size_t bit_pos = j % 8;
                    value_t val;
                    if (bitmap_data[byte_idx] & (1 << bit_pos)) {
                        val.str_val.table_id = table_id;
                        val.str_val.col_id = col_id;
                        val.str_val.page_id = idx;
                        val.str_val.offset = end_offset_idx;
                        results[cur_row].push_back(val);
                        end_offset_idx += 2;
                        continue;
                    }
                    results[cur_row].push_back(val);
                }
            }
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
            const auto& val = result[row_idx][col_idx];
            if (val.is_null) [[unlikely]] {
                if (attr_vec[col_idx] == DataType::INT32) {
                    inserter_int32.insert_null();
                } else if (attr_vec[col_idx] == DataType::VARCHAR) {
                    inserter_str.insert_null();
                }
            } else {
                
                if (val.is_string) {
                    // fetch string from corresponding page
                    auto& table = plan.inputs[val.str_val.table_id];
                    auto& column = table.columns[val.str_val.col_id];
                    Page* page = column.pages[val.str_val.page_id];
                    const uint8_t* page_data = reinterpret_cast<const uint8_t*>(page->data);
                    size_t offset = val.str_val.offset;
                    std::string str;
                    std::string_view sview{nullptr, 0};
                    uint16_t nr;
                    memcpy(&nr, &page_data[0], sizeof(uint16_t));
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
                    inserter_int32.insert(val.int32_val);
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