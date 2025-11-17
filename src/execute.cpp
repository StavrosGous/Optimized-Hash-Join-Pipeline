#include <hardware.h>
#include <plan.h>
#include <table.h>
#include "RHMap.h"
#include "CuckooMap.h"
#include "HopscotchMap.h"
#include <iostream>
#include <cmath>
#include "bitmap.h"

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

        // build phase: populate hash table with indices from build_side
        for (auto&& [idx, record] : build_side | views::enumerate) {
            const auto& key = record[build_col];
            if (!key.is_null) {
                if (auto loc = hash_table.find(key.int32_val); loc == hash_table.end()) {
                    hash_table.emplace(key.int32_val, std::vector<size_t>(1, idx));
                } else {
                    loc->push_back(idx);
                }
            }
        }

        // probe phase: for each probe record, lookup matches and emit joined rows
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
                                new_record.emplace_back((*left_record_ptr)[col_idx]);
                            } else {
                                new_record.emplace_back((*right_record_ptr)[col_idx - left_record_ptr->size()]);
                            }
                        }
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
            size_t prev_row = 0;
            for (auto& page : pages) {
                // std::cout << "Processing INT32 page with data size " << PAGE_SIZE << std::endl;
                // bytes 0 and 1 are a 2 byte number corresponding to number of rows 
                uint16_t nr = static_cast<uint16_t>(page->data[0]);
                // bytes 2 and 3 are a 2 byte number corresponding to number of non-nulls
                uint16_t nv = static_cast<uint16_t>(page->data[2]);
                auto* bitmap = reinterpret_cast<const uint8_t*>(page->data + PAGE_SIZE - (nr + 7) / 8);
                for (size_t i = prev_row, k = 0; i < nr, k < nv; ++i) {
                    size_t bitmap_idx = i / 8;
                    size_t bit_idx = i % 8;
                    value_t val;
                    if (static_cast<uint8_t>(bitmap[bitmap_idx]) & (0x1 << bit_idx)) {
                        val.is_string = false;
                        val.is_null = true;
                        results[i].push_back(val);
                        continue;
                    }
                    val.is_string = false;
                    val.is_null = false;
                    val.int32_val = *reinterpret_cast<int32_t*>(&page->data[4 + k * 4]);
                    results[i].push_back(val);
                    k++;
                }
                prev_row += nr;
            }
            std::cout << "Finished INT32 column " << col_id << " of table " << table_id << std::endl;
            break;
        }
        case DataType::VARCHAR: {
            // locate the page that contains the requested row
            auto& pages = column.pages;
            size_t prev_row = 0;
            for (const auto& [idx, page] : pages | views::enumerate) {
                // std::cout << "Processing VARCHAR page with data size " << PAGE_SIZE << std::endl;
                // bytes 0 and 1 are a 2 byte number
                uint16_t nr = static_cast<uint16_t>(page->data[0]);
                uint16_t nchars = static_cast<uint16_t>(page->data[2]);
                auto* bitmap = reinterpret_cast<const uint8_t*>(page->data + PAGE_SIZE - (nr + 7) / 8);
                if (nr == 0xffff) {
                    value_t val;
                    val.is_string = true;
                    val.is_null = false;
                    val.str_val.table_id = table_id;
                    val.str_val.col_id = col_id;
                    val.str_val.page_id = idx;
                    val.str_val.offset = 4;
                    results[prev_row].push_back(val);
                    prev_row += 1;
                    continue;
                }
                else if (nr == 0xfffe) {
                    // continue to next page
                    continue;
                }
                size_t char_begin = 4 + nr * 2;
                uint16_t next_offset = *reinterpret_cast<uint16_t*>(&page->data[4]);
                for (size_t i = prev_row, k = 0; i < nr; ++i) {
                    size_t bitmap_idx = i / 8;
                    size_t bit_idx = i % 8;
                    value_t val;
                    if (static_cast<uint8_t>(bitmap[bitmap_idx]) & (0x1 << bit_idx)) {
                        val.is_string = true;
                        val.is_null = true;
                        results[i].push_back(val);
                        continue;
                    }
                    val.is_string = true;
                    val.is_null = false;
                    val.str_val.table_id = table_id;
                    val.str_val.col_id = col_id;
                    val.str_val.page_id = idx;
                    val.str_val.offset = char_begin;
                    results[i].push_back(val);
                    char_begin = next_offset;
                    next_offset = *reinterpret_cast<uint16_t*>(&page->data[4 + k * 2]);
                    k++;
                }
                prev_row += nr;
            }
            std::cout << "Finished VARCHAR column " << col_id << " of table " << table_id << std::endl;
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
    std::cout << "Executing scan on table " << table_id << " with " << input.num_rows << " rows." <<std::endl;
    for (auto& [col_idx, data_type] : output_attrs) {
        auto& column = input.columns[col_idx];
        build_column_inserter(table_id, col_idx, column, data_type, results);
    }
    return results;
}

ExecuteResult execute_impl(const Plan& plan, size_t node_idx) {
    auto& node = plan.nodes[node_idx];
    std::cout << "Executing node " << node_idx << std::endl;
    return std::visit(
        [&](const auto& value) {
            using T = std::decay_t<decltype(value)>;
            if constexpr (std::is_same_v<T, JoinNode>) {
                std::cout << "Executing join node with idx " << node_idx << std::endl;
                return execute_hash_join(plan, value, node.output_attrs);
            } else {
                std::cout << "Executing scan node with idx " << node_idx << std::endl;
                return execute_scan(plan, value, node.output_attrs);
            }
        },
        node.data);
}

ColumnarTable materialize_columnar_table(const Plan& plan, const ExecuteResult& result, const std::vector<DataType>& types) {
    ColumnarTable table;
    table.num_rows = result.size();
    for (size_t col_idx = 0; col_idx < types.size(); ++col_idx) {
        Column column(types[col_idx]);
        ColumnInserter<std::string> inserter_str{column};
        ColumnInserter<int32_t> inserter_int32{column};

        for (size_t row_idx = 0; row_idx < result.size(); ++row_idx) {
            const auto& val = result[row_idx][col_idx];
            if (val.is_null) {
                if (types[col_idx] == DataType::INT32) {
                    inserter_int32.insert_null();
                } else if (types[col_idx] == DataType::VARCHAR) {
                    inserter_str.insert_null();
                }
            } else {
                if (val.is_string && types[col_idx] == DataType::VARCHAR) {
                    // fetch string from corresponding page
                    auto& table = plan.inputs[val.str_val.table_id];
                    auto& column = table.columns[val.str_val.col_id];
                    auto& page = column.pages[val.str_val.page_id];
                    size_t offset = val.str_val.offset;
                    std::string str;
                    while (offset < PAGE_SIZE && static_cast<char>(page->data[offset]) != 0) {
                        str.push_back(static_cast<char>(page->data[offset]));
                        offset++;
                    }
                    inserter_str.insert(str);
                } else if (!val.is_string && types[col_idx] == DataType::INT32) {
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
    std::cout << "Executing plan with " << plan.nodes.size() << " nodes." << std::endl;
    for (size_t i = 0; i < plan.nodes.size(); ++i) {
        std::cout << "Node " << i << ": ";
        const auto& node = plan.nodes[i];
        std::visit(
            [&](const auto& value) {
                using T = std::decay_t<decltype(value)>;
                if constexpr (std::is_same_v<T, JoinNode>) {
                    std::cout << "JoinNode" << std::endl;
                } else {
                    std::cout << "ScanNode" << std::endl;
                }
            },
            node.data);
    }       
    auto ret        = execute_impl(plan, plan.root);
    std::vector<DataType> ret_types;
    std::cout << "eftasa" << std::endl;
    ret_types.reserve(plan.nodes[plan.root].output_attrs.size());
    std::cout << "eftasa" << std::endl;
    for (const auto& attr : plan.nodes[plan.root].output_attrs) {
        ret_types.push_back(std::get<1>(attr));
    }

    std::cout << "eftasa" << std::endl;
    return materialize_columnar_table(plan, ret, ret_types);
}

void* build_context() {
    return nullptr;
}

void destroy_context([[maybe_unused]] void* context) {}

} // namespace Contest