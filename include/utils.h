#pragma once

#include <cstdint>
#include <cstring>
#include <iostream>
#include <vector>
#include <atomic>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <sys/mman.h>

#ifndef SMALL_SIZE
#define SMALL_SIZE 4096
#endif
#ifndef LARGE_SIZE
#define LARGE_SIZE 65536
#endif
#ifndef CAPACITY
#define CAPACITY 64
#endif
#ifndef NUM_PARTITIONS
#define NUM_PARTITIONS 64
#endif
#ifndef LOG2_NUM_PARTITIONS
#define LOG2_NUM_PARTITIONS 6
#endif
#include "plan.h"
#include "attribute.h"

#define MAX_PER_BUFFER_ENTRY (PAGE_SIZE / sizeof(int64_t))

template <typename P>
static inline uint16_t read_u16(const P* p) {
    return *reinterpret_cast<const uint16_t*>(reinterpret_cast<const void*>(p));
}

template <typename P>
static inline int32_t read_i32(const P* p) {
    return *reinterpret_cast<const int32_t*>(reinterpret_cast<const void*>(p));
}

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
		return crc64(*(uint64_t*)&key) * 0x2545F4914F6CDD1D;
	} else {
		std::cerr << "Unsupported key type for hashing" << std::endl;
		return 0LL;
	}
}


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


struct SmallChunk {
    SmallChunk *next = nullptr;
    uint8_t data[SMALL_SIZE - sizeof(SmallChunk *)];
};

struct LargeChunk {
    LargeChunk *next = nullptr;
    uint8_t data[LARGE_SIZE - sizeof(LargeChunk *)];
};

template<typename ChunkType>
class BumpAllocator {
    uint8_t *current = nullptr;
    uint8_t *end = nullptr;
    ChunkType *chunks = nullptr;

public:
    ChunkType *get_chunks() {
        return chunks;
    }

    size_t free_space() {
        return end - current;
    }
    void add_space(ChunkType *chunk) {
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
    uint8_t* arena = nullptr;           // mmaped memory
    size_t arena_size = 0;              // mmaped size (bytes)
    size_t arena_capacity = 0;          // capacity in chunks
    std::atomic<size_t> next_idx{0};    // next chunk allocated
    
    static constexpr size_t CHUNK_SIZE = sizeof(LargeChunk);
    static constexpr size_t PAGE_SIZE_ALLOC = 4096;

public:
    GlobalAllocator() = default;
    
    GlobalAllocator(GlobalAllocator&& other) noexcept 
        : arena(other.arena), arena_size(other.arena_size),
          arena_capacity(other.arena_capacity), next_idx(other.next_idx.load()) {
        other.arena = nullptr;
        other.arena_size = 0;
        other.arena_capacity = 0;
        other.next_idx.store(0);
    }

    void reserve(size_t num_chunks) {
        if (num_chunks <= arena_capacity) return;
        
        // Calculate new size (round up to page boundary)
        size_t new_size = num_chunks * CHUNK_SIZE;
        new_size = (new_size + PAGE_SIZE_ALLOC - 1) & ~(PAGE_SIZE_ALLOC - 1);
        
        // Allocate contiguous region with mmap
        uint8_t* new_arena = static_cast<uint8_t*>(
            mmap(nullptr, new_size, PROT_READ | PROT_WRITE, 
                 MAP_PRIVATE | MAP_ANONYMOUS, -1, 0));
        
        if (new_arena == MAP_FAILED) {
            std::cerr << "mmap failed for " << new_size << " bytes" << std::endl;
            return;
        }
        
        size_t used_chunks = next_idx.load(std::memory_order_relaxed);
        if (arena && used_chunks > 0) {
            std::memcpy(new_arena, arena, used_chunks * CHUNK_SIZE);
        }
        
        // Free old arena
        if (arena) {
            munmap(arena, arena_size);
        }
        
        arena = new_arena;
        arena_size = new_size;
        arena_capacity = new_size / CHUNK_SIZE;
    }

    void reset() {
        next_idx.store(0, std::memory_order_relaxed);
    }

    LargeChunk* allocate() {
        size_t idx = next_idx.fetch_add(1, std::memory_order_relaxed);
        if (idx < arena_capacity) {
            return reinterpret_cast<LargeChunk*>(arena + idx * CHUNK_SIZE);
        }
        void* ptr = aligned_alloc(alignof(LargeChunk), CHUNK_SIZE);
        return static_cast<LargeChunk*>(ptr);
    }

    void free_all() {
        if (arena) {
            munmap(arena, arena_size);
            arena = nullptr;
        }
        arena_size = 0;
        arena_capacity = 0;
        next_idx.store(0, std::memory_order_relaxed);
    }
    
    ~GlobalAllocator() {
        free_all();
    }
    
    GlobalAllocator(const GlobalAllocator&) = delete;
    GlobalAllocator& operator=(const GlobalAllocator&) = delete;
};


struct ThreadLocalState {
    GlobalAllocator &level1;
    BumpAllocator<LargeChunk> level2;
    BumpAllocator<SmallChunk> level3[NUM_PARTITIONS];
    size_t counts[NUM_PARTITIONS] = {0};
    ThreadLocalState(GlobalAllocator &alloc) : level1(alloc) {}

    template<typename T>
    T* consume(uint64_t hash) {
        uint64_t part = hash >> (64 - LOG2_NUM_PARTITIONS);
        if (level3[part].free_space() < sizeof(T)) {
            if (level2.free_space() < sizeof(SmallChunk)) {
                LargeChunk *large_chunk = level1.allocate();
                level2.add_space(large_chunk);
            }
            level3[part].add_space(level2.allocate<SmallChunk>());
        }
        counts[part]++;
        return level3[part].allocate<T>();
    }
};

class ThreadPool {
    std::vector<std::thread> workers;
    std::function<void(size_t)> task;
    std::mutex mtx;
    std::condition_variable cv_start;
    std::condition_variable cv_done;
    std::atomic<size_t> active_count{0};
    size_t num_threads;
    bool shutdown = false;
    size_t generation = 0;  
    std::vector<size_t> worker_generation;  

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
            
            local_task(thread_id);
            
            if (active_count.fetch_sub(1, std::memory_order_acq_rel) == 1) {
                std::lock_guard<std::mutex> done_lock(mtx);
                cv_done.notify_one();
            }
        }
    }

public:
    ThreadPool(size_t n_threads) : num_threads(n_threads), worker_generation(n_threads, 0) {
        workers.reserve(num_threads);
        for (size_t i = 0; i < num_threads; ++i) {
            workers.emplace_back([this, i] { worker_loop(i); });
        }
    }

    ~ThreadPool() {
        std::unique_lock<std::mutex> lock(mtx);
        shutdown = true;
        lock.unlock();
        cv_start.notify_all();
        for (auto& w : workers) {
            w.join();
        }
    }


    void run_parallel(std::function<void(size_t)> task_func) {
        std::unique_lock<std::mutex> lock(mtx);
        task = std::move(task_func);
        active_count.store(num_threads, std::memory_order_release);
        ++generation;
        lock.unlock();
        
        cv_start.notify_all();
        
        lock.lock();
        cv_done.wait(lock, [this] { return active_count.load(std::memory_order_acquire) == 0; });
        lock.unlock();
    }

    size_t get_num_threads() const { return num_threads; }

    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;
};
