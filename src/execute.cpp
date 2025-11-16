#include <hardware.h>
#include <plan.h>
#include <table.h>
#include "RHMap.h"
#include "CuckooMap.h"
#include "HopscotchMap.h"
#include <iostream>

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
            std::visit(
                [&hash_table, idx = idx](const auto& key) {
                    using Tk = std::decay_t<decltype(key)>;
                    if constexpr (std::is_same_v<Tk, T>) {
                        if (auto loc = hash_table.find(key); loc == hash_table.end()) {
                            hash_table.emplace(key, std::vector<size_t>(1, idx));
                        } else {
                            loc->push_back(idx);
                        }
                    } else if constexpr (not std::is_same_v<Tk, std::monostate>) {
                        throw std::runtime_error("wrong type of field");
                    }
                },
                record[build_col]);
        }

        // probe phase: for each probe record, lookup matches and emit joined rows
        for (auto& probe_record : probe_side) {
            std::visit(
                [&](const auto& key) {
                    using Tk = std::decay_t<decltype(key)>;
                    if constexpr (std::is_same_v<Tk, T>) {
                        if (auto loc = hash_table.find(key); loc != hash_table.end()) {
                            for (auto build_idx : *loc) {
                                const auto& build_record = build_side[build_idx];
                                // determine left/right pointers for output assembly
                                const auto* left_record_ptr = build_left ? &build_record : &probe_record;
                                const auto* right_record_ptr = build_left ? &probe_record : &build_record;
                                std::vector<Data> new_record;
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
                    } else if constexpr (not std::is_same_v<Tk, std::monostate>) {
                        throw std::runtime_error("wrong type of field");
                    }
                },
                probe_record[probe_col]);
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
        case DataType::VARCHAR: join_algorithm.run<std::string>(); break;
        }
    } else {
        switch (std::get<1>(right_types[join.right_attr])) {
        case DataType::INT32:   join_algorithm.run<int32_t>(); break;
        // case DataType::INT64:   join_algorithm.run<int64_t>(); break;
        // case DataType::FP64:    join_algorithm.run<double>(); break;
        case DataType::VARCHAR: join_algorithm.run<std::string>(); break;
        }
    }

    return results;
}

ExecuteResult execute_scan(const Plan&               plan,
    const ScanNode&                                  scan,
    const std::vector<std::tuple<size_t, DataType>>& output_attrs) {
    auto                           table_id = scan.base_table_id;
    auto&                          input    = plan.inputs[table_id];
    return Table::copy_scan(input, output_attrs);
}

ExecuteResult execute_impl(const Plan& plan, size_t node_idx) {
    auto& node = plan.nodes[node_idx];
    return std::visit(
        [&](const auto& value) {
            using T = std::decay_t<decltype(value)>;
            if constexpr (std::is_same_v<T, JoinNode>) {
                return execute_hash_join(plan, value, node.output_attrs);
            } else {
                return execute_scan(plan, value, node.output_attrs);
            }
        },
        node.data);
}

ColumnarTable execute(const Plan& plan, [[maybe_unused]] void* context) {
    namespace views = ranges::views;
    auto ret        = execute_impl(plan, plan.root);
    auto ret_types  = plan.nodes[plan.root].output_attrs
                   | views::transform([](const auto& v) { return std::get<1>(v); })
                   | ranges::to<std::vector<DataType>>();
    Table table{std::move(ret), std::move(ret_types)};
    return table.to_columnar();
}

void* build_context() {
    return nullptr;
}

void destroy_context([[maybe_unused]] void* context) {}

} // namespace Contest