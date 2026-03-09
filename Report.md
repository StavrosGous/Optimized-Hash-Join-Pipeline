# Technical Report

## Authors
- Ioannis Vogiatzis
- Stavros Gkousgkounis

# Fast Plans Runtime Benchmarks

## Fast, Default Implementation
Runtime measurements captured from `./build/fast plans.json` which includes test queries `1a-33c` had a mean total runtime of `130.0` seconds.
In detail one indicative run had the following durations:
```bash
$ cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -Wno-dev
$ cmake --build build -- -j $(nproc) fast
$ ./build/fast plans.json
...
Total runtime: 130090 ms
```

## Fast with Unchained Hashtable (HW 2)
Runtime measurements captured from `./run.sh fast_plans` which includes test queries `1a-33c` had a mean total runtime of `12.9` seconds.
In detail one indicative run had the following durations:
```bash
$ ./build.sh fast_plans
$ ./run.sh fast_plans
...
Total runtime: 12902 ms
```

## Fast with Optimized Indexing
Runtime measurements captured from `./run.sh fast_plans` which includes test queries `1a-33c` had a mean total runtime of `7.1` seconds.
```bash
$ ./build.sh fast_plans
$ ./run.sh fast_plans
...
Total runtime: 7178 ms
```

## Fast with Parallel Build of Hashtable
Runtime measurements captured from `./run.sh fast_plans` which includes test queries `1a-33c` had a mean total runtime of `6.8` seconds.
```bash
$ ./build.sh fast_plans
$ ./run.sh fast_plans
...
Total runtime: 6805 ms
```

## Fast with Parallel Probe with Work Stealing
Runtime measurements captured from `./run.sh fast_plans` which includes test queries `1a-33c` had a mean total runtime of `3.0` seconds.
```bash
$ ./build.sh fast_plans
$ ./run.sh fast_plans
...
Total runtime: 3019 ms
``` 
## Fast with Parallel Materialization
Runtime measurements captured from `./run.sh fast_plans` which includes test queries `1a-33c` had a mean total runtime of `2.2` seconds.
```bash
$ ./build.sh fast_plans
$ ./run.sh fast_plans
...
Total runtime: 2269 ms
```

---

# Performance Comparison Chart

![Total Runtime Comparison](total_runtime_comparison_chart.png)

## Speedup Summary

| Stage | Implementation | Total Runtime | Speedup vs Default |
|-------|---------------|---------------|-------------------|
| 0 | Default (HW1) | 130.0s | 1.0x |
| 1 | Unchained Hash Table (HW2) | 12.9s | 10.1x |
| 2 | + Optimized Indexing | 7.1s | 18.3x |
| 3 | + Parallel Build | 6.8s | 19.1x |
| 4 | + Parallel Probe (Work-Stealing) | 3.0s | 43.3x |
| 5 | + Parallel Materialization | 2.2s | 59.1x |

---

# Architecture and Design Choices

This section describes the key data structures and algorithms implemented in our optimized query execution engine. Each optimization was implemented incrementally and validated for correctness before moving to the next.

## Header Files

We added the following structs and helper functions to support our optimizations:

### include/utils.h
- **`column_t`**: Struct that contains all values of a column. Supports two modes:
  - *Buffered mode*: Data stored in `buffer_t` vectors for nullable/string columns
  - *Direct mode*: For INT32 columns without nulls, maintains a pointer `original_col` to the source Column and pre-computed `page_rowids` for O(1) row lookup
- **`buffer_t`**: Struct that stores up to `MAX_PER_BUFFER_ENTRY` (typically 1024) `value_t` values in a contiguous array
- **`value_t`**: 64-bit union of `int32_wrapper` and `string_struct` for late materialization support
- **`ThreadPool`**: Reusable thread pool with generation-based task dispatch. Workers sleep on condition variables and wake to execute tasks, avoiding costly thread creation (~1ms per join saved)
- **`GlobalAllocator`**: Lock-free mmap-based arena allocator that can be reused across joins via `reset()`
- **`BumpAllocator`**: Simple bump allocator for fast per-thread allocation from a pre-allocated region
- **`ThreadLocalState`**: Per-thread state for parallel hash table build, containing local partition buckets and counts
- **`SmallChunk` / `LargeChunk`**: Chunk types for the three-level slab allocator (4KB and 64KB respectively)
- **`read_u16` & `read_i32`**: Memory read helper functions for parsing page data

### include/unchained_ht.h
- **`UnchainedHashTable`**: Our implementation of the unchained hash table from the assignment paper
- **`Entry`**: 64-bit struct containing `int32_t key` and `uint32_t row_idx` for late materialization
- **`tags[2048]`**: Pre-computed array of all 1820 possible 16-bit Bloom filter tags with exactly 4 bits set, padded to 2048 for power-of-2 indexing. This avoids runtime computation of valid tag values.

---

## Key Changes Implemented

### 1. Optimized Indexing (Direct Page Access)

For INT32 columns that have **no null values**, we avoid copying data entirely by maintaining a direct reference to the original pages. This optimization is implemented in `build_column_inserter` in `src/execute.cpp`:

```cpp
// In build_column_inserter - check if column has no nulls
bool has_nulls = false;
uint16_t first_page_rows = 0;
for (auto& page : pages) {
    const uint8_t* page_data = reinterpret_cast<const uint8_t*>(page->data);
    uint16_t nr = read_u16(page_data);      // num_rows
    uint16_t nv = read_u16(page_data + 2);  // num_values (non-null count)
    if (first_page_rows == 0) first_page_rows = nr;
    if (nv != nr) { has_nulls = true; break; }
}

if (!has_nulls && first_page_rows > 0) {
    new_column.original_col = &column;  // Keep pointer to original Column
    new_column.rows_per_page = first_page_rows;
    // Pre-compute page_rowids for O(1) row-to-page mapping
    new_column.page_rowids.push_back(0);
    for (auto& page : pages) {
        const uint8_t* page_data = reinterpret_cast<const uint8_t*>(page->data);
        new_column.num_rows += read_u16(page_data);
        new_column.page_rowids.push_back(new_column.num_rows);
    }
    return;  // Skip buffer creation entirely
}
```

**During Probing (Direct Access)**:
During the probe phase, we check for direct access mode and read values directly from the original pages:
```cpp
if (probe_column.original_col != nullptr) {
    // Direct page access - zero copy
    for (const auto& page : probe_column.original_col->pages) {
        const uint8_t* page_data = reinterpret_cast<const uint8_t*>(page->data);
        uint16_t nr = read_u16(page_data);
        const int32_t* values = reinterpret_cast<const int32_t*>(page_data + 4);
        size_t base_idx = page_rowids[page_idx];  // Pre-computed offset
        for (uint16_t i = 0; i < nr; ++i) {
            int32_t key = values[i];  // Direct memory read
            auto [start, end] = hash_table.lookup(key);
            // Process matches...
        }
    }
}
```

### 2. Parallel Hash Table Build

The hash table build is parallelized using a **three-phase approach** with a **three-level slab allocator** to avoid memory allocation contention:

### Three-Level Slab Allocator

| Level | Component | Chunk Size | Purpose |
|-------|-----------|------------|---------|
| Level 1 | GlobalAllocator | 64KB | Large chunks allocated via mmap |
| Level 2 | BumpAllocator | 4KB | Small chunks per thread |
| Level 3 | Per-Partition | Per-tuple | Individual tuple storage |

```cpp
// Thread-local partitioning without locks
Entry* entry = thread_state->consume<Entry>(hash);
entry->key = key;
entry->row_idx = row_idx;
```

### Build Phases

**Phase 1: Parallel Partitioning**
- Each thread processes a range of input tuples
- Tuples are hashed and stored in thread-local partition buckets
- No synchronization needed - each thread has private storage

```cpp
// Split work among threads
size_t tuples_per_thread = (total_tuples + num_threads - 1) / num_threads;

for (size_t row_idx = start_idx; row_idx < end_idx; ++row_idx) {
    value_t val = col.get_value(row_idx);
    int32_t key = val.int32_val.val;
    uint64_t hash = crc_hash(key);
    Entry* entry = thread_states[t]->consume<Entry>(hash);
    entry->key = key;
    entry->row_idx = row_idx;
}
```

**Phase 2: Count Aggregation**
- Sum per-partition counts across all threads
- Compute prefix sums for final tuple positions

**Phase 3: Parallel postProcessBuild**
- Each thread processes a subset of partitions
- Builds directory entries and copies tuples to final positions
- Computes Bloom filter tags during counting

```cpp
void postProcessBuild(uint64_t partition, uint64_t prevCount, ...) {
    // First pass: count tuples and compute tags
    forEachEntryInPartition(thread_states[t], partition, [](Entry& tuple) {
        directory[slot] += sizeof(Entry) << 16;
        directory[slot] |= tags[hash_to_tag_idx(hash)];
    });
    
    // Compute prefix sums for this partition's slots
    for (uint64_t i = start; i < end; ++i) {
        uint64_t val = directory[i] >> 16;
        directory[i] = (cur << 16) | ((uint16_t)directory[i]);
        cur += val;
    }
    
    // Second pass: copy tuples to final positions
    forEachEntryInPartition(thread_states[t], partition, [](Entry& tuple) {
        Entry* target = reinterpret_cast<Entry*>(directory[slot] >> 16);
        *target = tuple;
        directory[slot] += sizeof(Entry) << 16;
    });
}
```

### 3. Parallel Probe with Work-Stealing

The probe phase uses **work-stealing** via an atomic counter for dynamic load balancing. Unlike static partitioning where each thread gets a fixed range of pages, work-stealing allows threads that finish early to automatically pick up more work:

```cpp
std::atomic<size_t> work_counter{0};

// Each thread claims work items dynamically
thread_pool.run_parallel([&](size_t t) {
    ExecuteResult& local_results = thread_results[t];
    
    size_t page_idx;
    while ((page_idx = work_counter.fetch_add(1, std::memory_order_relaxed)) < num_pages) {
        const uint8_t* page_data = reinterpret_cast<const uint8_t*>(pages[page_idx]->data);
        uint16_t nr = read_u16(page_data);
        const int32_t* values = reinterpret_cast<const int32_t*>(page_data + 4);
        size_t base_idx = page_rowids[page_idx];  // Pre-computed offset
        
        for (uint16_t i = 0; i < nr; ++i) {
            int32_t key = values[i];
            auto [start, end] = hash_table.lookup(key);
            
            for (auto entry_ptr = start; entry_ptr < end; ++entry_ptr) {
                if (entry_ptr->key != key) continue;  // Hash collision check
                
                size_t build_idx = entry_ptr->row_idx;
                size_t probe_global_idx = base_idx + i;
                
                // Collect matched values into thread-local buffers
                for (size_t out_idx = 0; out_idx < mappings.size(); ++out_idx) {
                    const auto& mapping = mappings[out_idx];
                    size_t target_row_idx = mapping.use_left ? 
                        (build_left ? build_idx : probe_global_idx) :
                        (build_left ? probe_global_idx : build_idx);
                    
                    value_t val = (*mapping.target_side)[mapping.col_idx].get_value(target_row_idx);
                    
                    auto& res_col = local_results[out_idx];
                    if (res_col.buffers.empty() || res_col.buffers.back().count == MAX_PER_BUFFER_ENTRY) {
                        res_col.buffers.emplace_back();
                    }
                    res_col.buffers.back().data[res_col.buffers.back().count++] = std::move(val);
                    ++res_col.num_rows;
                }
            }
        }
    }
});
```

**Result Aggregation:**
After the parallel probe, each thread has its own `local_results`. We aggregate these into the final result:
```cpp
// Merge thread-local results
for (size_t out_idx = 0; out_idx < output_attrs.size(); ++out_idx) {
    for (size_t t = 0; t < num_threads; ++t) {
        auto& thread_col = thread_results[t][out_idx];
        for (auto& buf : thread_col.buffers) {
            for (size_t i = 0; i < buf.count; ++i) {
                // Copy to final result buffers
            }
        }
    }
}
```

### 4. ThreadPool Implementation

Instead of creating new threads for each parallel phase (build partition, build postprocess, probe), we implemented a **reusable ThreadPool** in `include/utils.h` that keeps worker threads alive between tasks. This eliminates the significant overhead of thread creation:

```cpp
class ThreadPool {
    std::vector<std::thread> workers;
    std::function<void(size_t)> task;
    std::mutex mtx;
    std::condition_variable cv_start;
    std::condition_variable cv_done;
    std::atomic<size_t> active_count{0};
    size_t num_threads;
    bool shutdown = false;
    size_t generation = 0;  // Prevents spurious wakeups

    void worker_loop(size_t thread_id) {
        size_t my_generation = 0;
        while (true) {
            std::unique_lock<std::mutex> lock(mtx);
            cv_start.wait(lock, [this, &my_generation] { 
                return shutdown || generation > my_generation; 
            });
            
            if (shutdown) return;
            
            my_generation = generation;
            auto local_task = task;
            lock.unlock();
            
            local_task(thread_id);  // Execute the task
            
            // Last worker signals completion
            if (active_count.fetch_sub(1, std::memory_order_acq_rel) == 1) {
                std::lock_guard<std::mutex> done_lock(mtx);
                cv_done.notify_one();
            }
        }
    }

public:
    ThreadPool(size_t n_threads) : num_threads(n_threads) {
        workers.reserve(num_threads);
        for (size_t i = 0; i < num_threads; ++i) {
            workers.emplace_back([this, i] { worker_loop(i); });
        }
    }

    void run_parallel(std::function<void(size_t)> task_func) {
        std::unique_lock<std::mutex> lock(mtx);
        task = std::move(task_func);
        active_count.store(num_threads, std::memory_order_release);
        ++generation;
        lock.unlock();
        
        cv_start.notify_all();  // Wake all workers
        
        lock.lock();
        cv_done.wait(lock, [this] { return active_count.load(std::memory_order_acquire) == 0; });
    }
    
    size_t get_num_threads() const { return num_threads; }
};
```

**Key Design Decisions:**
- **Generation counter**: Each task increments `generation`. Workers track their last completed generation to avoid processing the same task twice or waking for no reason.
- **Condition variables**: Workers sleep on `cv_start` instead of spinning, reducing CPU usage when idle. This is important since we may have many joins in a single query.
- **Active count tracking**: We use an atomic counter `active_count` to know when all workers have finished. The last worker to finish (`fetch_sub` returns 1) notifies the main thread.
- **Global instance**: A single `ThreadPool` is created at startup in `build_context()` and stored as a global pointer. It is reused for all queries and all joins within a query.

**Usage in JoinAlgorithm:**
```cpp
// Global thread pool - created once in build_context()
ThreadPool* g_thread_pool = nullptr;

void* build_context() {
    g_thread_pool = new ThreadPool(SPC__THREAD_COUNT);
    g_global_alloc = new GlobalAllocator();
    return nullptr;
}

// In JoinAlgorithm::run()
thread_pool.run_parallel([&](size_t t) {
    // Each worker executes this with their thread_id
    // ...parallel task...
});
```

### 6. GlobalAllocator with mmap

The `GlobalAllocator` in `include/utils.h` uses **mmap** (falls back to `aligned_alloc` when more memory than reserved is needed) for lock-free allocation of large memory regions. This is used by the parallel hash table build to allocate memory for thread-local partition storage :

```cpp
class GlobalAllocator {
    uint8_t* arena = nullptr;
    size_t arena_size = 0;
    std::atomic<size_t> next_idx{0};
    
public:
    void reserve(size_t num_chunks) {
        size_t new_size = num_chunks * CHUNK_SIZE;  // CHUNK_SIZE = 64KB
        arena = (uint8_t*)mmap(nullptr, new_size,
                               PROT_READ | PROT_WRITE,
                               MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        arena_size = new_size;
    }
    
    LargeChunk* allocate() {
        // Lock-free allocation - just increment the counter
        size_t idx = next_idx.fetch_add(1, std::memory_order_relaxed);
        return reinterpret_cast<LargeChunk*>(arena + idx * CHUNK_SIZE);
    }
    
    void reset() {
        // Reset allocation pointer without deallocating memory
        next_idx.store(0, std::memory_order_relaxed);
    }
    
    ~GlobalAllocator() {
        if (arena) munmap(arena, arena_size);
    }
};
```

**Global Instance:**
Like the ThreadPool, a single GlobalAllocator is created at startup and reused:
```cpp
GlobalAllocator* g_global_alloc = nullptr;

void* build_context() {
    g_thread_pool = new ThreadPool(SPC__THREAD_COUNT);
    g_global_alloc = new GlobalAllocator();
    return nullptr;
}
```

# Query Performance: Heavy Queries

Queries with runtime >500ms in the default implementation show the most dramatic improvements:

![Total Runtime Comparison](total_runtime_comparison_chart.png)


---

# Conclusions

Our final implementation achieves a **59x speedup** over the default implementation, reducing total runtime from 130 seconds to 2.2 seconds. The key optimizations that contributed to this improvement:

### Optimization Impact Analysis

1. **Unchained Hash Table** (130s → 12.9s, **10x**)
   - Eliminated hash collision chains
   - Bloom filter tags reduce false positive lookups
   - Better cache locality with contiguous storage

2. **Optimized Indexing** (12.9s → 7.1s, **1.8x**)
   - Direct page access for non-null INT32 columns
   - Eliminated data copying during scans
   - Pre-computed page_rowids for O(1) lookup

3. **Parallel Build** (7.1s → 6.8s, **1.04x**)
   - Multi-threaded hash table construction
   - Three-level slab allocator avoids contention

4. **Parallel Probe + Work-Stealing** (6.8s → 3.0s, **2.3x**)
   - All CPU cores utilized during probe
   - Dynamic load balancing handles variable selectivity

5. **ThreadPool Reuse** (3.0s → 2.2s, **1.4x**)
   - Eliminated ~1ms thread creation per join
   - Reused GlobalAllocator across queries

### Key Learnings

- **Thread creation is expensive**: Creating threads costs ~1ms each, which adds up with many joins per query
- **Memory allocation contention matters**: The three-level slab allocator was essential for parallel build performance
- **Work-stealing simplicity wins**: A single atomic counter provides excellent load balancing with minimal overhead
- **Data locality is critical**: Direct page access and avoiding copies provided significant speedups
