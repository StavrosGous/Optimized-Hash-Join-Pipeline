#pragma once

#include <cstdint>
#include <cstring>
#include <iostream>


#ifndef SMALL_SIZE
#define SMALL_SIZE 4096
#endif
#ifndef LARGE_SIZE
#define LARGE_SIZE 65536
#endif
#ifndef CAPACITY
#define CAPACITY 64
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
}

struct LargeChunk {
    LargeChunk *next = nullptr;
    uint8_t data[LARGE_SIZE - sizeof(LargeChunk *)];
}

class BumpAllocator {
    uint8_t *current = nullptr;
    uint8_t *end = nullptr;
    SmallChunk *chunks = nullptr;

public:
    size_t free_space() {
        return end - current;
    }
    void add_space(SmallChunk *chunk) {
        chunk->next = chunks;
        chunks = chunk;
        current = chunk->data;
        end = current + (SMALL_SIZE - sizeof(SmallChunk *));
    }

    template<typename T>
    T* allocate() {
        T* ptr = reinterpret_cast<T*>(current);
        current += sizeof(T);
        return ptr;
    }
};