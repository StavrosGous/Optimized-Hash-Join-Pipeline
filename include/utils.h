#pragma once

#include <atomic>
#include <condition_variable>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <mutex>
#include <thread>
#include <vector>
#include <sys/mman.h>

#include "plan.h"
#include "attribute.h"

// Allocator chunk sizes
static constexpr size_t SMALL_CHUNK_SIZE = 4096;
static constexpr size_t LARGE_CHUNK_SIZE = 65536;

// Partitioning constants
static constexpr size_t NUM_PARTITIONS = 64;
static constexpr size_t LOG2_NUM_PARTITIONS = 6;

#define MAX_PER_BUFFER_ENTRY (PAGE_SIZE / sizeof(int64_t))

// ---------------------------------------------------------------------------
// Memory read helpers
// ---------------------------------------------------------------------------

template <typename P>
static inline uint16_t read_u16(const P* p) {
    return *reinterpret_cast<const uint16_t*>(reinterpret_cast<const void*>(p));
}

template <typename P>
static inline int32_t read_i32(const P* p) {
    return *reinterpret_cast<const int32_t*>(reinterpret_cast<const void*>(p));
}

// ---------------------------------------------------------------------------
// CRC32 hashing (hardware-accelerated)
// ---------------------------------------------------------------------------

static inline uint64_t crc64(uint64_t key) {
    return __builtin_ia32_crc32di(key, 0);
}

static inline uint32_t crc32(uint32_t key) {
    return __builtin_ia32_crc32si(key, 0);
}

template<typename T>
uint64_t crc_hash(const T& key) {
    if constexpr (std::is_same_v<T, int32_t>) {
        return crc32(key) * ((0x8648DBDull << 32) + 1);
    } else if constexpr (std::is_same_v<T, int64_t>) {
        return crc64(key) * 0x2545F4914F6CDD1D;
    } else if constexpr (std::is_same_v<T, double>) {
        uint64_t bits;
        std::memcpy(&bits, &key, sizeof(bits));
        return crc64(bits) * 0x2545F4914F6CDD1D;
    } else {
        static_assert(!std::is_same_v<T, T>, "Unsupported key type for hashing");
    }
}


// ---------------------------------------------------------------------------
// Value types for late materialization
// ---------------------------------------------------------------------------

struct string_struct {
    uint16_t table_id;
    uint16_t col_id;
    uint16_t page_id;
    uint16_t offset;
};

struct int32_wrapper {
    int32_t val;
    int32_t status;
};

union value_t {
    int32_wrapper int32_val;
    string_struct str_val;
};

struct buffer_t {
    value_t* data;
    uint16_t count;

    buffer_t() {
        data = new value_t[MAX_PER_BUFFER_ENTRY];
        count = 0;
    }
    ~buffer_t() {
        if (data) delete[] data;
    }
    buffer_t(buffer_t&& other) noexcept {
        data = other.data;
        count = other.count;
        other.data = nullptr;
        other.count = 0;
    }
    buffer_t& operator=(buffer_t&& other) noexcept {
        if (this != &other) {
            if (data) delete[] data;
            data = other.data;
            count = other.count;
            other.data = nullptr;
            other.count = 0;
        }
        return *this;
    }
    buffer_t(const buffer_t&) = delete;
    buffer_t& operator=(const buffer_t&) = delete;
};


struct column_t {
    std::vector<buffer_t> buffers;
    size_t                num_rows;
    DataType              type;
    
    const Column*  original_col;  
    uint16_t       rows_per_page;
    std::vector<size_t> page_rowids;

    column_t() : num_rows(0), type(DataType::INT32), original_col(nullptr), rows_per_page(0) {}
    column_t(const DataType& dt) : num_rows(0), type(dt), original_col(nullptr), rows_per_page(0) {}
    
    inline value_t get_value(size_t row_idx) const {
        if (original_col == nullptr) {
            size_t buf_idx = row_idx / MAX_PER_BUFFER_ENTRY;
            size_t buf_offset = (row_idx % MAX_PER_BUFFER_ENTRY);
            return buffers[buf_idx].data[buf_offset];
        } else {
            size_t page_idx = row_idx / rows_per_page;
            size_t offset_in_page = row_idx % rows_per_page;
            const uint8_t* page_data = reinterpret_cast<const uint8_t*>(original_col->pages[page_idx]->data);
            value_t val;
            val.int32_val.val = read_i32(page_data + 4 + 4 * offset_in_page);
            val.int32_val.status = 1;
            return val;
        }
    }
};


// ---------------------------------------------------------------------------
// Three-level slab allocator
//   Level 1: GlobalAllocator  — mmap arena, lock-free, allocates LargeChunks
//   Level 2: BumpAllocator    — carves SmallChunks from LargeChunks
//   Level 3: Per-partition     — carves entries from SmallChunks
// ---------------------------------------------------------------------------

struct SmallChunk {
    SmallChunk* next = nullptr;
    uint8_t data[SMALL_CHUNK_SIZE - sizeof(SmallChunk*)];
};

struct LargeChunk {
    LargeChunk* next = nullptr;
    uint8_t data[LARGE_CHUNK_SIZE - sizeof(LargeChunk*)];
};

template<typename ChunkType>
class BumpAllocator {
    uint8_t* current = nullptr;
    uint8_t* end = nullptr;
    ChunkType* chunks = nullptr;

public:
    ChunkType* get_chunks() const { return chunks; }
    size_t free_space() const { return static_cast<size_t>(end - current); }

    void add_space(ChunkType* chunk) {
        chunk->next = chunks;
        chunks = chunk;
        current = chunk->data;
        end = current + sizeof(chunk->data);
    }

    template<typename T>
    T* allocate() {
        T* ptr = reinterpret_cast<T*>(current);
        current += sizeof(T);
        return ptr;
    }
};

class GlobalAllocator {
    uint8_t* arena_ = nullptr;
    size_t arena_size_ = 0;
    size_t arena_capacity_ = 0;  // in chunks
    std::atomic<size_t> next_idx_{0};

    static constexpr size_t CHUNK_SIZE = sizeof(LargeChunk);
    static constexpr size_t MMAP_PAGE_SIZE = 4096;

public:
    GlobalAllocator() = default;

    GlobalAllocator(GlobalAllocator&& other) noexcept
        : arena_(other.arena_), arena_size_(other.arena_size_),
          arena_capacity_(other.arena_capacity_),
          next_idx_(other.next_idx_.load(std::memory_order_relaxed)) {
        other.arena_ = nullptr;
        other.arena_size_ = 0;
        other.arena_capacity_ = 0;
        other.next_idx_.store(0, std::memory_order_relaxed);
    }

    ~GlobalAllocator() { free_all(); }

    GlobalAllocator(const GlobalAllocator&) = delete;
    GlobalAllocator& operator=(const GlobalAllocator&) = delete;

    /// Pre-allocate a contiguous mmap arena for at least `num_chunks` chunks.
    void reserve(size_t num_chunks) {
        if (num_chunks <= arena_capacity_) return;

        size_t new_size = num_chunks * CHUNK_SIZE;
        new_size = (new_size + MMAP_PAGE_SIZE - 1) & ~(MMAP_PAGE_SIZE - 1);

        uint8_t* new_arena = static_cast<uint8_t*>(
            mmap(nullptr, new_size, PROT_READ | PROT_WRITE,
                 MAP_PRIVATE | MAP_ANONYMOUS, -1, 0));

        if (new_arena == MAP_FAILED) {
            std::perror("GlobalAllocator::reserve mmap failed");
            std::abort();
        }

        // Copy live data from old arena
        size_t used = next_idx_.load(std::memory_order_relaxed);
        if (arena_ && used > 0) {
            std::memcpy(new_arena, arena_, used * CHUNK_SIZE);
        }
        if (arena_) munmap(arena_, arena_size_);

        arena_ = new_arena;
        arena_size_ = new_size;
        arena_capacity_ = new_size / CHUNK_SIZE;
    }

    /// Reset the allocation pointer without freeing memory (reuse across joins).
    void reset() { next_idx_.store(0, std::memory_order_relaxed); }

    /// Lock-free allocation of a single LargeChunk.
    LargeChunk* allocate() {
        size_t idx = next_idx_.fetch_add(1, std::memory_order_relaxed);
        if (idx < arena_capacity_) {
            return reinterpret_cast<LargeChunk*>(arena_ + idx * CHUNK_SIZE);
        }
        // Fallback: arena exhausted — allocate from heap
        void* ptr = aligned_alloc(alignof(LargeChunk), CHUNK_SIZE);
        return static_cast<LargeChunk*>(ptr);
    }

    void free_all() {
        if (arena_) {
            munmap(arena_, arena_size_);
            arena_ = nullptr;
        }
        arena_size_ = 0;
        arena_capacity_ = 0;
        next_idx_.store(0, std::memory_order_relaxed);
    }
};


/// Per-thread state for parallel hash table build.
/// Each thread partitions tuples into local buckets without synchronization.
struct ThreadLocalState {
    GlobalAllocator& level1;
    BumpAllocator<LargeChunk> level2;
    BumpAllocator<SmallChunk> level3[NUM_PARTITIONS];
    size_t counts[NUM_PARTITIONS] = {0};

    explicit ThreadLocalState(GlobalAllocator& alloc) : level1(alloc) {}

    template<typename T>
    T* consume(uint64_t hash) {
        uint64_t part = hash >> (64 - LOG2_NUM_PARTITIONS);
        if (level3[part].free_space() < sizeof(T)) {
            if (level2.free_space() < sizeof(SmallChunk)) {
                level2.add_space(level1.allocate());
            }
            level3[part].add_space(level2.allocate<SmallChunk>());
        }
        counts[part]++;
        return level3[part].allocate<T>();
    }
};

// ---------------------------------------------------------------------------
// Reusable thread pool with generation-based task dispatch.
// Workers sleep on a condition variable between tasks, avoiding the ~1ms
// overhead of thread creation/destruction per join.
// ---------------------------------------------------------------------------

class ThreadPool {
    std::vector<std::thread> workers_;
    std::function<void(size_t)> task_;
    std::mutex mtx_;
    std::condition_variable cv_start_;
    std::condition_variable cv_done_;
    std::atomic<size_t> active_count_{0};
    size_t num_threads_;
    bool shutdown_ = false;
    size_t generation_ = 0;

    void worker_loop(size_t thread_id) {
        size_t my_generation = 0;
        while (true) {
            std::unique_lock<std::mutex> lock(mtx_);
            cv_start_.wait(lock, [this, &my_generation] {
                return shutdown_ || generation_ > my_generation;
            });

            if (shutdown_) return;

            my_generation = generation_;
            auto local_task = task_;
            lock.unlock();

            local_task(thread_id);

            if (active_count_.fetch_sub(1, std::memory_order_acq_rel) == 1) {
                std::lock_guard<std::mutex> done_lock(mtx_);
                cv_done_.notify_one();
            }
        }
    }

public:
    explicit ThreadPool(size_t n_threads) : num_threads_(n_threads) {
        workers_.reserve(num_threads_);
        for (size_t i = 0; i < num_threads_; ++i) {
            workers_.emplace_back([this, i] { worker_loop(i); });
        }
    }

    ~ThreadPool() {
        {
            std::lock_guard<std::mutex> lock(mtx_);
            shutdown_ = true;
        }
        cv_start_.notify_all();
        for (auto& w : workers_) w.join();
    }

    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;

    /// Dispatch `task_func(thread_id)` to all workers and block until complete.
    void run_parallel(std::function<void(size_t)> task_func) {
        {
            std::lock_guard<std::mutex> lock(mtx_);
            task_ = std::move(task_func);
            active_count_.store(num_threads_, std::memory_order_release);
            ++generation_;
        }
        cv_start_.notify_all();

        std::unique_lock<std::mutex> lock(mtx_);
        cv_done_.wait(lock, [this] {
            return active_count_.load(std::memory_order_acquire) == 0;
        });
    }

    size_t get_num_threads() const { return num_threads_; }
};
