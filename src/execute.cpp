#include <hardware.h>
#include <plan.h>
#include <table.h>
#include <iostream>
#include <cmath>
#include <string_view>
#include <thread>
#include <vector>
#include <atomic>
#include "unchained_ht.h"


namespace Contest {

using ExecuteResult = std::vector<column_t>;
ExecuteResult execute_impl(const Plan&, size_t, ThreadPool&);
inline void build_column_inserter(const size_t, const size_t, const Column&, DataType, column_t&);
ColumnarTable materialize_columnar_table_parallel(const Plan&, const ExecuteResult&, const std::vector<DataType>&, ThreadPool&);

struct JoinAlgorithm {
    ExecuteResult&                                   left;
    ExecuteResult&                                   right;
    ExecuteResult&                                   results;
    size_t                                           left_col, right_col;
    const std::vector<std::tuple<size_t, DataType>>& output_attrs;
    ThreadPool&                                      thread_pool;

    template <class T>
    auto run() {
        namespace views = ranges::views;
        
        const auto& left_col_val = left[left_col];
        const auto& right_col_val = right[right_col];

        if (left_col_val.num_rows == 0 || right_col_val.num_rows == 0) {
            results.resize(output_attrs.size());
            return;
        }

        bool build_left = (left_col_val.num_rows <= right_col_val.num_rows);
        auto const &build_column = build_left ? left_col_val : right_col_val;
        auto const &probe_column = build_left ? right_col_val : left_col_val;

        // build hash_table
        UnchainedHashTable hash_table(build_column.num_rows);
        hash_table.build_parallel(build_column, thread_pool);
        
        // helper struct to map the output columns to the corresponding input columns once before the join
        struct OutputMapping {
            const ExecuteResult* target_side; // pointer to either left or right target side
            size_t col_idx;
            bool use_left; // 1 for row index usage, 0 for right
        };
        std::vector<OutputMapping> mappings;
        mappings.reserve(output_attrs.size());
        for (size_t out_idx =0 ; out_idx < output_attrs.size(); ++out_idx) {
            auto col_idx = std::get<0>(output_attrs[out_idx]);
            if (col_idx < left.size()) {
                mappings.push_back({&left, col_idx, true});
            } else {
                mappings.push_back({&right, col_idx - left.size(), false});
            }
        }

        size_t num_threads = thread_pool.get_num_threads();
        
        std::vector<ExecuteResult> thread_results(num_threads);
        for (size_t t = 0; t < num_threads; ++t) {
            thread_results[t].resize(output_attrs.size());
        }

        if (probe_column.original_col != nullptr) {
            const auto& pages = probe_column.original_col->pages;
            size_t num_pages = pages.size();
            // Use pre-calculated page_rowids from column creation
            const auto& page_rowids = probe_column.page_rowids;
            
            std::atomic<size_t> work_counter{0};
            
            thread_pool.run_parallel([&](size_t t) {
                ExecuteResult& local_results = thread_results[t];
                
                size_t page_idx;
                while ((page_idx = work_counter.fetch_add(1, std::memory_order_relaxed)) < num_pages) {
                    const uint8_t* page_data = reinterpret_cast<const uint8_t*>(pages[page_idx]->data);
                    uint16_t nr = read_u16(page_data);
                    const int32_t* values = reinterpret_cast<const int32_t*>(page_data + 4);
                    size_t base_idx = page_rowids[page_idx];
                    
                    for (uint16_t i = 0; i < nr; ++i) {
                        int32_t key = values[i];
                        auto [start, end] = hash_table.lookup(key);
                        for (auto entry_ptr = start; entry_ptr < end; ++entry_ptr) {
                            if (entry_ptr->key != key) continue;
                            size_t build_idx = entry_ptr->row_idx;
                            size_t probe_global_idx = base_idx + i;
                            size_t left_row_idx = build_left ? build_idx : probe_global_idx;
                            size_t right_row_idx = build_left ? probe_global_idx : build_idx;
                            
                            for (size_t out_idx = 0; out_idx < mappings.size(); ++out_idx) {
                                const auto& mapping = mappings[out_idx];
                                size_t target_row_idx = mapping.use_left ? left_row_idx : right_row_idx;
                                const auto& side = (*mapping.target_side)[mapping.col_idx];
                                value_t val = side.get_value(target_row_idx);
                                
                                auto& res_col = local_results[out_idx];
                                if (res_col.buffers.empty() || (res_col.buffers.back().count == MAX_PER_BUFFER_ENTRY)) {
                                    res_col.buffers.emplace_back();
                                }
                                auto& last_buf = res_col.buffers.back();
                                last_buf.data[last_buf.count++] = std::move(val);
                                ++res_col.num_rows;
                            }
                        }
                    }
                }
            });
        } else {
            size_t num_buffers = probe_column.buffers.size();
            
            std::atomic<size_t> work_counter{0};
            
            thread_pool.run_parallel([&](size_t t) {
                ExecuteResult& local_results = thread_results[t];
                
                size_t buf_idx;
                while ((buf_idx = work_counter.fetch_add(1, std::memory_order_relaxed)) < num_buffers) {
                    const auto& probe_buffer = probe_column.buffers[buf_idx];
                    for (size_t i = 0; i < probe_buffer.count; ++i) {
                        value_t probe_val = probe_buffer.data[i];
                        int32_t key = probe_val.int32_val.val;
                        if (probe_val.int32_val.status) {
                            auto [start, end] = hash_table.lookup(key);
                            for (auto entry_ptr = start; entry_ptr < end; ++entry_ptr) {
                                if (entry_ptr->key != key) continue;
                                size_t build_idx = entry_ptr->row_idx;
                                size_t probe_global_idx = buf_idx * MAX_PER_BUFFER_ENTRY + i;
                                size_t left_row_idx = build_left ? build_idx : probe_global_idx;
                                size_t right_row_idx = build_left ? probe_global_idx : build_idx;
                                
                                for (size_t out_idx = 0; out_idx < mappings.size(); ++out_idx) {
                                    const auto& mapping = mappings[out_idx];
                                    size_t target_row_idx = mapping.use_left ? left_row_idx : right_row_idx;
                                    const auto& side = (*mapping.target_side)[mapping.col_idx];
                                    value_t val = side.get_value(target_row_idx);

                                    auto& res_col = local_results[out_idx];
                                    if (res_col.buffers.empty() || (res_col.buffers.back().count == MAX_PER_BUFFER_ENTRY)) {
                                        res_col.buffers.emplace_back();
                                    }
                                    auto& last_buf = res_col.buffers.back();
                                    last_buf.data[last_buf.count++] = std::move(val);
                                    ++res_col.num_rows;
                                }
                            }
                        }
                    }
                }
            });
        }
        
        ExecuteResult final_results;
        final_results.resize(output_attrs.size());
        
         for (size_t out_idx = 0; out_idx < output_attrs.size(); ++out_idx) {
            size_t total_rows = 0;
            for (size_t t = 0; t < num_threads; ++t) {
                total_rows += thread_results[t][out_idx].num_rows;
            }
            
            if (total_rows == 0) {
                continue;
            }
            
            // Pre-allocate all buffers needed
            size_t num_full_buffers = total_rows / MAX_PER_BUFFER_ENTRY;
            size_t remainder = total_rows % MAX_PER_BUFFER_ENTRY;
            size_t total_buffers = num_full_buffers + (remainder > 0 ? 1 : 0);
            final_results[out_idx].buffers.reserve(total_buffers);
            
            buffer_t current_buf;
            
            for (size_t t = 0; t < num_threads; ++t) {
                auto& thread_col = thread_results[t][out_idx];
                for (auto& buf : thread_col.buffers) {
                    for (size_t i = 0; i < buf.count; ++i) {
                        if (current_buf.count == MAX_PER_BUFFER_ENTRY) {
                            final_results[out_idx].buffers.push_back(std::move(current_buf));
                            current_buf = buffer_t{};
                        }
                        current_buf.data[current_buf.count++] = buf.data[i];
                    }
                }
            }
            
            if (current_buf.count > 0) {
                final_results[out_idx].buffers.push_back(std::move(current_buf));
            }
            
            final_results[out_idx].num_rows = total_rows;
        }
        
        results = std::move(final_results);
    }
};

ExecuteResult execute_hash_join(const Plan&          plan,
    const JoinNode&                                  join,
    const std::vector<std::tuple<size_t, DataType>>& output_attrs,
    ThreadPool& thread_pool) {
    auto                           left_idx    = join.left;
    auto                           right_idx   = join.right;
    auto&                          left_node   = plan.nodes[left_idx];
    auto&                          right_node  = plan.nodes[right_idx];
    auto&                          left_attr_vec  = left_node.output_attrs;
    auto&                          right_attr_vec = right_node.output_attrs;
    auto                           left        = execute_impl(plan, left_idx, thread_pool);
    auto                           right       = execute_impl(plan, right_idx, thread_pool);
    std::vector<column_t> results;
    results.reserve(std::min(left.size() + right.size(), static_cast<size_t>(left.size() * 2)));

    JoinAlgorithm join_algorithm{
        .left                                = left,
        .right                               = right,
        .results                             = results,
        .left_col                            = join.left_attr,
        .right_col                           = join.right_attr,
        .output_attrs                        = output_attrs,
        .thread_pool                         = thread_pool};
    join_algorithm.run<int32_t>();
    return std::move(results);
}



ExecuteResult execute_scan(const Plan&               plan,
    const ScanNode&                                  scan,
    const std::vector<std::tuple<size_t, DataType>>& output_attrs) {
    
    auto                           table_id = scan.base_table_id;
    auto&                          input    = plan.inputs[table_id];
    ExecuteResult                results(output_attrs.size());
    for (size_t idx=0; idx < output_attrs.size(); ++idx) {
        auto col_idx = std::get<0>(output_attrs[idx]);
        auto& column = input.columns[col_idx];
        DataType data_type = column.type;
        results[idx] = column_t(data_type);
        build_column_inserter(table_id, col_idx, column, data_type, results[idx]);
    }
    return std::move(results);
}

ExecuteResult execute_impl(const Plan& plan, size_t node_idx, ThreadPool& thread_pool) {
    auto& node = plan.nodes[node_idx];
    auto ret = (std::visit(
        [&](const auto& value) {
            using T = std::decay_t<decltype(value)>;
            if constexpr (std::is_same_v<T, JoinNode>) {
                return std::move(execute_hash_join(plan, value, node.output_attrs, thread_pool));
            } else {
                return std::move(execute_scan(plan, value, node.output_attrs));
            }
        },
        node.data));
    return std::move(ret);
}


ColumnarTable execute(const Plan& plan, [[maybe_unused]] void* context) {
    namespace views = ranges::views;
    ThreadPool& thread_pool = *static_cast<ThreadPool*>(context);
    auto ret        = execute_impl(plan, plan.root, thread_pool);
    std::vector<DataType> ret_attr_vec;
    ret_attr_vec.reserve(plan.nodes[plan.root].output_attrs.size());
    for (const auto& attr : plan.nodes[plan.root].output_attrs) {
        ret_attr_vec.push_back(std::move(std::get<1>(attr)));
    }
    ColumnarTable table = materialize_columnar_table_parallel(plan, ret, ret_attr_vec, thread_pool);
    return table;
}

void* build_context() {
    size_t num_threads = SPC__THREAD_COUNT;
    if (num_threads == 0) num_threads = 4;
    return new ThreadPool(num_threads);
}

void destroy_context([[maybe_unused]] void* context) {
    if (context) {
        delete static_cast<ThreadPool*>(context);
    }
}



// function to build our column_t from Column of the ColumnarTable 
inline void build_column_inserter(const size_t table_id, const size_t col_id, const Column& column, DataType data_type, column_t& new_column) {
    namespace views = ranges::views;
    auto& pages = column.pages;
    std::vector<buffer_t> buffers;
    buffers.reserve(pages.size());
    buffer_t curbuf;
    switch (data_type) {
        case DataType::INT32: {
            bool has_nulls = false;
            uint16_t first_page_rows = 0;
            for (auto& page : pages) {
                const uint8_t* page_data = reinterpret_cast<const uint8_t*>(page->data);
                uint16_t nr = read_u16(page_data);
                uint16_t nv = read_u16(page_data + 2);
                if (first_page_rows == 0) first_page_rows = nr;
                if (nv != nr) {
                    has_nulls = true;
                    break;
                }
            }
            
            if (!has_nulls && first_page_rows > 0) {
                new_column.original_col = &column;
                new_column.rows_per_page = first_page_rows;
                new_column.num_rows = 0;
                new_column.page_rowids.reserve(pages.size() + 1);
                new_column.page_rowids.push_back(0);
                for (auto& page : pages) {
                    const uint8_t* page_data = reinterpret_cast<const uint8_t*>(page->data);
                    uint16_t nr = read_u16(page_data);
                    new_column.num_rows += nr;
                    new_column.page_rowids.push_back(new_column.num_rows);
                }
                return;
            }
            
            for (auto& page : pages) {
                const uint8_t* page_data = reinterpret_cast<const uint8_t*>(page->data);
                uint16_t nr = read_u16(page_data);
                size_t bitmap_size = (nr + 7) / 8;
                const uint8_t* bitmap_data = reinterpret_cast<const uint8_t*>(page_data) + PAGE_SIZE - bitmap_size;
                size_t cur_nv_row = 0;
                for (size_t i = 0; i < nr; ++i) {
                    size_t byte_idx = i / 8;
                    size_t bit_pos = i % 8;
                    value_t wrapper_val;
                    if (bitmap_data[byte_idx] & (1 << bit_pos)) {
                        wrapper_val.int32_val.val = read_i32(page_data + 4 + 4 * (cur_nv_row++));
                        wrapper_val.int32_val.status = 1; //indicate non null value
                    }
                    else {
                        wrapper_val.int32_val.status = 0; // indicate null value
                    }
                    curbuf.data[curbuf.count++] = std::move(wrapper_val);
                    if (curbuf.count == MAX_PER_BUFFER_ENTRY) {
                        buffers.push_back(std::move(curbuf));
                        curbuf.data = new value_t[MAX_PER_BUFFER_ENTRY];
                        curbuf.count = 0;
                    }
                }
            }
            break;
        }
        case DataType::VARCHAR: {
            
            for (size_t idx = 0; idx < pages.size(); ++idx) {
                auto& page = pages[idx];
                const uint8_t* page_data = reinterpret_cast<const uint8_t*>(page->data);
                uint16_t nr = read_u16(page_data);

                if (nr == 0xffff) {
                    value_t val;
                    val.str_val.table_id = table_id;
                    val.str_val.col_id = col_id;
                    val.str_val.page_id = idx;
                    val.str_val.offset = 4;
                    curbuf.data[curbuf.count++] = std::move(val);
                    if (curbuf.count == MAX_PER_BUFFER_ENTRY) {
                        buffers.push_back(std::move(curbuf));
                        curbuf.data = new value_t[MAX_PER_BUFFER_ENTRY];
                        curbuf.count = 0;
                    }
                    continue;
                }
                else if (nr == 0xfffe) {
                    continue;
                }

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
                    curbuf.data[curbuf.count++] = std::move(val);
                    if (curbuf.count == MAX_PER_BUFFER_ENTRY) {
                        buffers.push_back(std::move(curbuf));
                        curbuf.data = new value_t[MAX_PER_BUFFER_ENTRY];
                        curbuf.count = 0;
                    }
                }
            }
            break;
        }
    }
    if (curbuf.count > 0) {
        buffers.push_back(std::move(curbuf));
    }
    new_column.buffers = std::move(buffers);
    if (new_column.buffers.empty()) {
        new_column.num_rows = 0;
    } else {
        new_column.num_rows = (new_column.buffers.size() - 1) * MAX_PER_BUFFER_ENTRY + new_column.buffers.back().count;
    }
   
}


// function to materialize the final ExecuteResult into the corresponding ColumnarTable (parallel version)
ColumnarTable materialize_columnar_table_parallel(const Plan& plan, const ExecuteResult& result, const std::vector<DataType>& attr_vec, ThreadPool& thread_pool) {
    ColumnarTable table;
    table.num_rows = result.empty() ? 0 : result[0].num_rows;
    size_t num_threads = thread_pool.get_num_threads();
    
    for (size_t col_idx = 0; col_idx < attr_vec.size(); ++col_idx) {
        Column column(attr_vec[col_idx]);
        const column_t& col = result[col_idx];
        
        if (col.num_rows == 0) {
            ColumnInserter<std::string> inserter_str{column};
            ColumnInserter<int32_t> inserter_int32{column};
            for (size_t row_idx = 0; row_idx < table.num_rows; ++row_idx) {
                if (attr_vec[col_idx] == DataType::INT32) {
                    inserter_int32.insert_null();
                } else if (attr_vec[col_idx] == DataType::VARCHAR) {
                    inserter_str.insert_null();
                }
            }
            inserter_int32.finalize();
            inserter_str.finalize();
        } else {
            std::vector<std::vector<Page*>> thread_pages(num_threads);
            
            thread_pool.run_parallel([&](size_t thread_id) {
                Column local_column(attr_vec[col_idx]);
                ColumnInserter<std::string> inserter_str{local_column};
                ColumnInserter<int32_t> inserter_int32{local_column};
                
                size_t rows_per_thread = (table.num_rows + num_threads - 1) / num_threads;
                size_t start_row = thread_id * rows_per_thread;
                size_t end_row = std::min(start_row + rows_per_thread, table.num_rows);
                
                for (size_t row_idx = start_row; row_idx < end_row; ++row_idx) {
                    value_t val = col.get_value(row_idx);
                    if (val.str_val.offset == 0xFFFF || !val.int32_val.status) {
                        if (attr_vec[col_idx] == DataType::INT32) {
                            inserter_int32.insert_null();
                        } else if (attr_vec[col_idx] == DataType::VARCHAR) {
                            inserter_str.insert_null();
                        }
                    } else {
                        if (attr_vec[col_idx] == DataType::VARCHAR) {
                            auto& table = plan.inputs[val.str_val.table_id];
                            auto& column = table.columns[val.str_val.col_id];
                            Page* page = column.pages[val.str_val.page_id];
                            const uint8_t* page_data = reinterpret_cast<const uint8_t*>(page->data);
                            size_t offset = val.str_val.offset;
                            std::string str;
                            std::string_view sview{nullptr, 0};
                            uint16_t nr = read_u16(page_data);
                            uint16_t page_id = val.str_val.page_id;
                            // if the string is long, we build the string page by page
                            if (nr == 0xffff) {
                                uint16_t nchars = read_u16(page_data + 2);
                                str.assign(reinterpret_cast<const char*>(page_data + 4), nchars);
                                ++page_id;
                                page = column.pages[page_id];
                                page_data = reinterpret_cast<const uint8_t*>(page->data);
                                nr = read_u16(page_data);
                                while (nr == 0xfffe) [[likely]] {
                                    nchars = read_u16(page_data + 2);
                                    str.append(reinterpret_cast<const char*>(page_data + 4), nchars);
                                    ++page_id;
                                    page = column.pages[page_id];
                                    page_data = reinterpret_cast<const uint8_t*>(page->data);
                                    nr = read_u16(page_data);
                                }
                                inserter_str.insert(str);
                                continue;
                            }
                            // if the string is short, we use string_view to avoid copying it like above
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
                            inserter_str.insert(sview);
                        } else {
                            inserter_int32.insert(val.int32_val.val);
                        }
                    }
                }
                inserter_int32.finalize();
                inserter_str.finalize();
                thread_pages[thread_id] = std::move(local_column.pages);
            });
            
            for (size_t t = 0; t < num_threads; ++t) {
                for (auto& page : thread_pages[t]) {
                    column.pages.push_back(page);
                }
            }
        }
        table.columns.push_back(std::move(column));
    }
    return table;
}


} // namespace Contest