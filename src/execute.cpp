#include <hardware.h>
#include <plan.h>
#include <table.h>
#include "RHMap.h"
#include "CuckooMap.h"
#include "HopscotchMap.h"
#include <iostream>
#include <cmath>
// #include "bitmap.h"

#ifndef HASH_MAP
#define HASH_MAP 1
#endif

namespace Contest {


struct string_struct {
    uint16_t table_id;
    uint16_t col_id;
    uint16_t page_id;
    uint16_t offset;
};

struct value_t {
    bool is_string;
    bool is_null;
    union {
        int32_t       int32_val;
        string_struct str_val;
    };
};

// using ExecuteResult = std::vector<std::vector<Data>>;
using ExecuteResult = std::vector<std::vector<value_t>>;

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

        hashmap_type hash_table(build_side.size() * 1.6); // preallocate with some extra space
        // std::cout << "Building hash table with " << build_side.size() << " entries." << std::endl;
        // build phase: populate hash table with indices from build_side
        for (auto&& [idx, record] : build_side | views::enumerate) {
            const auto& key = record[build_col];
            // std::cout << "key: " << key.int32_val << std::endl;
            // Cache key value to avoid redundant access
            const auto& key_value = key.int32_val;
            if (!key.is_null) {
                if (auto loc = hash_table.find(key_value); loc == hash_table.end()) {
                    hash_table.emplace(key_value, std::vector<size_t>(1, idx));
                    // Debug: Log keys being inserted into the hash table
                    // std::cout << "Building hash table with key: " << key.int32_val << ", index: " << idx << std::endl;
                } else {
                    loc->push_back(idx);
                }
            }
        }

        // probe phase: for each probe record, lookup matches and emit joined rows
        for (auto& probe_record : probe_side) {
            const auto& key = probe_record[probe_col];
            // std::cout << "key: " << key.int32_val << std::endl;
            if (!key.is_null) {
                // Debug: Log keys being probed
                // std::cout << "Probing hash table with key: " << key.int32_val << std::endl;
                if (auto loc = hash_table.find(key.int32_val); loc != hash_table.end()) {
                    for (auto build_idx : *loc) {
                        // Debug: Log matches found
                        // std::cout << "Match found for key: " << key.int32_val << ", build index: " << build_idx << std::endl;
                        const auto& build_record = build_side[build_idx];
                        const auto* left_record_ptr = build_left ? &build_record : &probe_record;
                        const auto* right_record_ptr = build_left ? &probe_record : &build_record;
                        std::vector<value_t> new_record;
                        new_record.reserve(output_attrs.size());
                        for (auto [col_idx, _] : output_attrs) {
                            if (col_idx < left_record_ptr->size()) {
                                new_record.emplace_back((*left_record_ptr)[col_idx]);
                            } else {
                                new_record.emplace_back((*right_record_ptr)[col_idx - left_record_ptr->size()]);
                            }
                        }
                        // std::cout << "Emitting joined record with " << new_record.size() << " columns." << std::endl;
                        results.emplace_back(std::move(new_record));
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
    auto&                          left_types  = left_node.output_attrs;
    auto&                          right_types = right_node.output_attrs;
    auto                           left        = execute_impl(plan, left_idx);
    auto                           right       = execute_impl(plan, right_idx);
    // print left adn right tables
    // std::cout << "Left table has " << left.size() << " rows with idx " << left_idx << std::endl;
    // std::cout << "Right table has " << right.size() << " rows with idx " << right_idx << std::endl;
    std::vector<std::vector<value_t>> results;

    JoinAlgorithm join_algorithm{.build_left = join.build_left,
        .left                                = left,
        .right                               = right,
        .results                             = results,
        .left_col                            = join.left_attr,
        .right_col                           = join.right_attr,
        .output_attrs                        = output_attrs};
    if (join.build_left) {
        switch (std::get<1>(left_types[join.left_attr])) {
        case DataType::INT32:   join_algorithm.run<int32_t>(); break;
        // case DataType::INT64:   join_algorithm.run<int64_t>(); break;
        // case DataType::FP64:    join_algorithm.run<double>(); break;
        // case DataType::VARCHAR: join_algorithm.run<std::string>(); break;
        }
    } else {
        switch (std::get<1>(right_types[join.right_attr])) {
        case DataType::INT32:   join_algorithm.run<int32_t>(); break;
        // case DataType::INT64:   join_algorithm.run<int64_t>(); break;
        // case DataType::FP64:    join_algorithm.run<double>(); break;
        // case DataType::VARCHAR: join_algorithm.run<std::string>(); break;
        }
    }

    return results;
}



// fetch value from columns
void build_column_inserter(const size_t table_id, const size_t col_id, const Column& column, DataType data_type, ExecuteResult& results) {
    namespace views = ranges::views;
    switch (data_type) {
        case DataType::INT32: {
            // locate the page that contains the requested row
            auto& pages = column.pages;
            int cur_row = 0;
            for (auto& page : pages) {
                // std::cout << "Processing INT32 page with data size " << PAGE_SIZE << std::endl;
                // bytes 0 and 1 are a 2 byte number corresponding to number of rows 
                uint16_t nr;
                memcpy(&nr, &page->data[0], sizeof(uint16_t));

                // bytes 2 and 3 are a 2 byte number corresponding to number of non-nulls
                uint16_t nv;
                memcpy(&nv, &page->data[2], sizeof(uint16_t));

                // Cache frequently used values to reduce redundant calculations
                size_t bitmap_size = (nr + 7) / 8;
                const uint8_t* bitmap_data = reinterpret_cast<const uint8_t*>(page->data) + PAGE_SIZE - bitmap_size;
                size_t base_offset = 4;

                for (size_t i = 0, j = 0; j < nr; ++j, ++cur_row) {
                    size_t byte_idx = j / 8;
                    size_t bit_pos = j % 8;
                    value_t val;

                    if (bitmap_data[byte_idx] & (1 << bit_pos)) {
                        val.is_null = false;
                        val.is_string = false;
                        memcpy(&val.int32_val, &page->data[base_offset + i * 4], sizeof(int32_t));
                        results[cur_row].push_back(val);
                        ++i;
                        continue;
                    }

                    val.is_null = true;
                    val.is_string = false;
                    results[cur_row].push_back(val);
                }
            }
            // std::cout << "Finished INT32 column " << col_id << " of table " << table_id << std::endl;
            break;
        }
        case DataType::VARCHAR: {
            // locate the page that contains the requested row
            auto& pages = column.pages;
            size_t cur_row = 0;
            for (const auto& [idx, page] : pages | views::enumerate) {
                // Corrected the type conversion for page_data
                const uint8_t* page_data = reinterpret_cast<const uint8_t*>(page->data);

                // Safely fetch 2-byte numbers using memcpy
                uint16_t nr;
                memcpy(&nr, &page_data[0], sizeof(uint16_t));

                uint16_t nchars;
                memcpy(&nchars, &page_data[2], sizeof(uint16_t));
                uint16_t nv = nchars;

                // Fetch bitmap safely using memcpy
                size_t bitmap_size = (nr + 7) / 8;
                const uint8_t* bitmap_data = page_data + PAGE_SIZE - bitmap_size;

                if (nr == 0xffff) {
                    value_t val;
                    val.is_string = true;
                    val.is_null = false;
                    val.str_val.table_id = table_id;
                    val.str_val.col_id = col_id;
                    val.str_val.page_id = idx;
                    val.str_val.offset = 4;
                    results[cur_row].push_back(val);
                    cur_row++;
                    continue;
                }
                else if (nr == 0xfffe) {
                    // continue to next page
                    continue;
                }
                // Cache frequently used values to reduce redundant calculations
                const uint8_t* bitmap_data_cached = reinterpret_cast<const uint8_t*>(page->data) + PAGE_SIZE - bitmap_size;
                uint16_t end_offset_idx = 4;

                for (size_t i = 0, j = 0; j < nr; ++j, ++cur_row) {
                    size_t byte_idx = j / 8;
                    size_t bit_pos = j % 8;
                    value_t val;

                    if (bitmap_data_cached[byte_idx] & (1 << bit_pos)) {
                        val.is_string = true;
                        val.is_null = false;
                        val.str_val.table_id = table_id;
                        val.str_val.col_id = col_id;
                        val.str_val.page_id = idx;
                        val.str_val.offset = end_offset_idx;
                        // print start_offset, end_offset and the string
                        // uint16_t start_offset;
                        // uint16_t end_offset;
                        // if (end_offset_idx > 4) {
                        //     memcpy(&start_offset, &page->data[end_offset_idx - 2], sizeof(uint16_t));
                        //     memcpy(&end_offset, &page->data[end_offset_idx], sizeof(uint16_t));
                        //     start_offset += 4 + nr * 2;
                        //     end_offset += 4 + nr * 2;
                        //     // build string for debugging
                        //     std::string str;
                        //     for (size_t k = start_offset; k < end_offset; ++k) {
                        //         str.push_back(static_cast<char>(page->data[k]));
                        //     }
                        //     // offsets
                        //     // std::cout << "Start offset: " << start_offset << ", End offset: " << end_offset <<  " with nr "<< nr << std::endl;
                        //     // std::cout << "String: " << str << std::endl;
                        // }
                        // else {
                        //     start_offset = 4 + nr * 2;
                        //     memcpy(&end_offset, &page->data[4], sizeof(uint16_t));
                        //     end_offset += start_offset;
                        //     // build string for debugging
                        //     std::string str;
                        //     for (size_t k = start_offset; k < end_offset; ++k) {
                        //         str.push_back(static_cast<char>(page->data[k]));
                        //     }
                        //     // offsets
                        //     // std::cout << "Start offset: " << start_offset << ", End offset: " << end_offset <<  " with nr "<< nr << std::endl;
                        //     // std::cout << "String: " << str << std::endl;
                        // }

                        results[cur_row].push_back(val);
                        end_offset_idx += 2;
                        i++;
                        continue;
                    }

                    val.is_string = true;
                    val.is_null = true;
                    results[cur_row].push_back(val);

                }
                
            }
            // std::cout << "Finished VARCHAR column " << col_id << " of table " << table_id << std::endl;
            break;
        }
    }
}

ExecuteResult execute_scan(const Plan&               plan,
    const ScanNode&                                  scan,
    const std::vector<std::tuple<size_t, DataType>>& output_attrs) {
    auto                           table_id = scan.base_table_id;
    auto&                          input    = plan.inputs[table_id];
    ExecuteResult                results(input.num_rows);
    // std::cout << "Executing scan on table " << table_id << " with " << input.num_rows << " rows." <<std::endl;
    for (auto& [col_idx, data_type] : output_attrs) {
        auto& column = input.columns[col_idx];
        // std::cout << "Building column inserter for column " << col_idx << " of table " << table_id << std::endl;
        build_column_inserter(table_id, col_idx, column, data_type, results);
        // std::cout << "Finished building column inserter for column " << col_idx << " of table " << table_id << std::endl;
    }
    return results;
}

ExecuteResult execute_impl(const Plan& plan, size_t node_idx) {
    auto& node = plan.nodes[node_idx];
    // std::cout << "Executing node " << node_idx << std::endl;
    return std::visit(
        [&](const auto& value) {
            using T = std::decay_t<decltype(value)>;
            if constexpr (std::is_same_v<T, JoinNode>) {
                // std::cout << "Executing join node with idx " << node_idx << std::endl;
                return execute_hash_join(plan, value, node.output_attrs);
            } else {
                // std::cout << "Executing scan node with idx " << node_idx << std::endl;
                return execute_scan(plan, value, node.output_attrs);
            }
        },
        node.data);
}

ColumnarTable materialize_columnar_table(const Plan& plan, const ExecuteResult& result, const std::vector<DataType>& types) {
    ColumnarTable table;
    table.num_rows = result.size();
    // std::cout << "Materializing columnar table with " << result.size() << " rows and " << types.size() << " columns." << std::endl;
    for (size_t col_idx = 0; col_idx < types.size(); ++col_idx) {
        Column column(types[col_idx]);
        ColumnInserter<std::string> inserter_str{column};
        ColumnInserter<int32_t> inserter_int32{column};
        // std::cout << "Materializing column " << col_idx << " with result.size() = " << result.size() << std::endl;
        for (size_t row_idx = 0; row_idx < result.size(); ++row_idx) {
            const auto& val = result[row_idx][col_idx];
            if (val.is_null) {
                if (types[col_idx] == DataType::INT32) {
                    // std::cout << "Finished materializing column " << col_idx <<  " with value NULL" << std::endl;
                    inserter_int32.insert_null();
                } else if (types[col_idx] == DataType::VARCHAR) {
                    // std::cout << "Finished materializing column " << col_idx <<  " with value NULL" << std::endl;
                    inserter_str.insert_null();
                }
            } else {
                if (val.is_string && types[col_idx] == DataType::VARCHAR) {
                    // fetch string from corresponding page
                    auto& table = plan.inputs[val.str_val.table_id];
                    auto& column = table.columns[val.str_val.col_id];
                    Page* page = column.pages[val.str_val.page_id];
                    size_t offset = val.str_val.offset;
                    std::string str;
                    // find end offset
                    uint16_t nr;
                    memcpy(&nr, &page->data[0], sizeof(uint16_t));
                    uint16_t page_id = val.str_val.page_id;
                    bool is_long = false;
                    // std::cout << "Materializing " << (is_long ? "long" : "short") << " string from table " << val.str_val.table_id << ", column " << val.str_val.col_id << ", page " << val.str_val.page_id << ", offset " << val.str_val.offset << std::endl;
                    while (nr == 0xffff || nr == 0xfffe) {
                        uint16_t nchars;
                        memcpy(&nchars, &page->data[2], sizeof(uint16_t));
                        // read nchars characters and continue to next page if it is a long string
                        for (size_t i = 4; i < nchars; ++i) {
                            str.push_back(static_cast<char>(page->data[i]));
                        }
                        // move to next page
                        page_id++;
                        memcpy(&nr, &column.pages[page_id]->data[0], sizeof(uint16_t));
                        page = column.pages[page_id];
                        is_long = true;
                    }
                    if (!is_long) {
                        uint16_t end_offset_pos = offset;
                        uint16_t end_offset;
                        uint16_t start_offset;
                        if (end_offset_pos > 4) {
                            memcpy(&end_offset, &page->data[end_offset_pos], sizeof(uint16_t));
                            memcpy(&start_offset, &page->data[end_offset_pos - 2], sizeof(uint16_t));
                            start_offset += 4 + nr * 2;
                            end_offset += 4 + nr * 2;

                        }
                        else {
                            start_offset = 4 + nr * 2;
                            memcpy(&end_offset, &page->data[4], sizeof(uint16_t));
                            end_offset += start_offset;
                        }
                        // from start_offset to end_offset is the string
                        for (size_t i = start_offset; i < end_offset; ++i) {
                            str.push_back(static_cast<char>(page->data[i]));
                        }
                        // std::cout << "Materializing " << (is_long ? "long" : "short") << " string from table " << val.str_val.table_id << ", column " << val.str_val.col_id << ", page " << val.str_val.page_id << ", start offset " << start_offset << " end offset " << end_offset << std::endl;
                        
                    }
                    // std::cout << "Finished materializing column " << col_idx <<  " with value " << str << std::endl;
                    
                    // std::cout << "String value: " << str << std::endl;
                    inserter_str.insert(str);
                } else if (!val.is_string && types[col_idx] == DataType::INT32) {
                    // std::cout << "Finished materializing column " << col_idx <<  " with value " << val.int32_val << std::endl;
                    inserter_int32.insert(val.int32_val);
                } else {
                    throw std::runtime_error("type mismatch when materializing columnar table");
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
    // print all nodes that should be processed
    // std::cout << "Executing plan with " << plan.nodes.size() << " nodes." << std::endl;
    // for (size_t i = 0; i < plan.nodes.size(); ++i) {
    //     std::cout << "Node " << i << ": ";
    //     const auto& node = plan.nodes[i];
    //     std::visit(
    //         [&](const auto& value) {
    //             using T = std::decay_t<decltype(value)>;
    //             if constexpr (std::is_same_v<T, JoinNode>) {
    //                 std::cout << "JoinNode" << std::endl;
    //             } else {
    //                 std::cout << "ScanNode" << std::endl;
    //             }
    //         },
    //         node.data);
    // }       
    auto ret        = execute_impl(plan, plan.root);
    std::vector<DataType> ret_types;
    ret_types.reserve(plan.nodes[plan.root].output_attrs.size());
    for (const auto& attr : plan.nodes[plan.root].output_attrs) {
        ret_types.push_back(std::get<1>(attr));
    }
    // std::cout << "eftasa" << std::endl;

    return materialize_columnar_table(plan, ret, ret_types);
}

void* build_context() {
    return nullptr;
}

void destroy_context([[maybe_unused]] void* context) {}

} // namespace Contest