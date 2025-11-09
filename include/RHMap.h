#pragma once

#include <cstdint>
#include <functional>
#include <ostream>
#include <utility>
#include <vector>
#include "utils.h"

template <typename T, typename T_r>
class RHMap;

template <typename T, typename T_r>
class Bucket {
private:
    T key;
    T_r val;
    size_t psl; // probe sequence length used by robin hood balancing
    bool is_occupied;
    size_t home_idx; // index where this key originally hashed to

public:
    Bucket() : key(), val(), psl(0), is_occupied(false), home_idx(0) {}

    void update(T &&key, T_r &&val, size_t &&psl, size_t &&home_idx) {
        this->key = std::move(key);
        this->val = std::move(val);
        this->psl = std::move(psl);
        this->is_occupied = true;
        this->home_idx = std::move(home_idx);
    }
    
    friend class RHMap<T, T_r>;

};

template <typename T, typename T_r>
class RHMap {
private:
    std::vector<Bucket<T, T_r>> b; // contiguous buckets backing the hash table
    size_t count; // number of occupied buckets
    size_t capacity;
    size_t mask; // capacity - 1, enabling cheap modulo via bitmask when capacity is power of two

    void rehash() {
        // resize to the next power-of-two bucket array so masks remain valid
        std::vector<Bucket<T, T_r>> old_buckets = b;
        b.clear();
        // grow by doubling
        capacity *= 2;
        b.resize(capacity);
        mask = capacity - 1;
        // reinsert existing entries to rebuild invariants
        count = 0;
        for (auto &old_bucket : old_buckets) {
            if (old_bucket.is_occupied) {
                emplace(std::move(old_bucket.key), std::move(old_bucket.val));
            }
        }
    }

    
public:
    RHMap() : count(0), capacity(CAPACITY),
         mask(this->capacity - 1)
    {
        b.resize(this->capacity); // preallocate power-of-two bucket array for mask arithmetic
    }
    RHMap(const size_t &buildsize) : count(0),
        capacity([&buildsize]() {
            size_t calculated = buildsize > 0 ? 1ULL << (sizeof(size_t) * 8 - __builtin_clzll(buildsize - 1)) : 1;
            if (buildsize * 10 > calculated * 7) {
                calculated <<= 1;
            }
            return calculated;
        }()), // round up to next power-of-two for mask arithmetic and grow early if close to load factor threshold
        mask(this->capacity - 1)
    {
        b.resize(this->capacity);
    }

    void emplace(T key, T_r val) {
        size_t local_mask = mask;
        size_t og_idx = crc_hash(key) & local_mask; // home position of the incoming key
        size_t idx = og_idx;
        size_t psl = 0; // probe sequence length of the incoming key

        Bucket<T, T_r>* bucket = &b[idx];
        while (bucket->is_occupied) { // robin hood swap when newcomer probed longer
            if (bucket->psl < psl) {
                std::swap(key, bucket->key);
                std::swap(val, bucket->val);
                std::swap(psl, bucket->psl);
                std::swap(og_idx, bucket->home_idx);
            }
            idx = (idx + 1) & local_mask;
            ++psl;
            bucket = &b[idx];
        }
        bucket->update(std::move(key), std::move(val), std::move(psl), std::move(og_idx));
        ++count;
        if (count * 10 > capacity * 7) {
            // keep average probe length bounded by expanding early
            rehash();
        }
    }
    

    T_r* end() { return nullptr; }

    T_r* find(const T& key) {
        size_t local_mask = mask;
        size_t idx = crc_hash(key) & local_mask; // start probing from home bucket
        size_t cpsl = 0; // current probe length while scanning the cluster

        Bucket<T, T_r>* bucket = &b[idx];
        while (bucket->is_occupied) {
            if (bucket->key == key) {
                return &bucket->val;
            }
            if (bucket->psl < cpsl) {
                // encountered a bucket that probed less: key cannot be further down the cluster
                break;
            }
            idx = (idx + 1) & local_mask;
            ++cpsl;
            bucket = &b[idx];
        }
        return end();
    }

};