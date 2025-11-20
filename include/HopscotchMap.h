#pragma once
#include <cstdint>
#include <utility>
#include <vector>
#include <chrono>
#include "utils.h"

#ifndef H
#define H 64
#endif


template<typename T, typename T_r>
class HopscotchMap;


template<typename T, typename T_r>
class HopBucket {
private:
    T key;
    T_r val;
    uint64_t bitmap;
    size_t home_idx;
    uint8_t sz; // neighbourhood size - small value fits in 8 bits
    unsigned is_occupied : 1; // packed flag for occupancy
    bool isFull() const {
        return sz == H;
    }

public:
    HopBucket() : key(), val(), bitmap(0), home_idx(0), sz(0), is_occupied(0) {}

    void update(T &&key, T_r &&val, size_t &&home_idx) {
        this->key = std::move(key);
        this->val = std::move(val);
        this->home_idx = std::move(home_idx);
        this->is_occupied = true;
    }
    friend class HopscotchMap<T, T_r>;
};


template<typename T, typename T_r>
class HopscotchMap {
private:
    std::vector<HopBucket<T, T_r>> b; // ring-buffer buckets giving us hop neighbourhoods
    size_t capacity;
    size_t mask;
    size_t count;
    
    void rehash() {
        std::vector<HopBucket<T, T_r>> old_Hopbuckets = std::move(b);
        b.clear();
        capacity *= 2;
        b.resize(capacity);
        mask = capacity - 1;
        for (auto &old_Hopbucket : old_Hopbuckets) {
            if (old_Hopbucket.is_occupied) {
                emplace(std::move(old_Hopbucket.key), std::move(old_Hopbucket.val));
            }
        }
    }


public:

    HopscotchMap() : capacity(CAPACITY),
        count(0),
        mask(this->capacity - 1)
    {
        b.resize(this->capacity);
    }

    HopscotchMap(const size_t &buildsize) : capacity(buildsize > CAPACITY ? 1ULL << (sizeof(size_t) * 8 - __builtin_clzll(buildsize - 1)) : CAPACITY),
        count(0),
        mask(this->capacity - 1)
    {
        b.resize(this->capacity);
    }

    void emplace(T key, T_r val) {
        size_t local_mask = mask;
        size_t og_idx = crc_hash(key) & local_mask; // hash of key to be inserted
        size_t idx = og_idx; // save original key hash for future reference
        HopBucket<T, T_r>* bucket = &b[idx];
        while (bucket->is_occupied) { // linear search to find empty Hopbucket
            idx = (idx + 1) & local_mask;
            bucket = &b[idx];
        }
        size_t pos = idx; // saving position of empty Hopbucket
        bucket->update(std::move(key), std::move(val), std::move(og_idx));
        ++count;
        size_t diff = (pos - og_idx) & local_mask; // distance of empty Hopbucket from original key hash
        while (diff >= H) { // if distance is at least H, need to hopscotch
            bool moved = false;
            for (size_t k = 1; k < H; ++k) { // Checking the previous H-1 Hopbuckets for possible trade
                const size_t i = (pos + capacity - k) & local_mask;
                HopBucket<T, T_r>* candidate = &b[i];
                if (!candidate->is_occupied) {
                    continue;
                }
                const size_t curhash = candidate->home_idx; // cached home slot avoids rehashing
                const size_t tempdiff = (pos - curhash) & local_mask; // calculate the distance between Hopbucket and candidate for trade
                if (tempdiff < H) { // if distance is less than H, we trade
                    // updating bitmap representation
                    HopBucket<T, T_r>* home = &b[curhash];
                    const size_t old_offset = (i - curhash) & local_mask;
                    const uint64_t new_bit = 1ULL << tempdiff;
                    const uint64_t old_bit = 1ULL << old_offset;
                    const bool new_set = (home->bitmap & new_bit) != 0;
                    const bool old_set = (home->bitmap & old_bit) != 0;
                    if (!new_set) {
                        home->bitmap |= new_bit;
                        home->sz++;
                    }
                    if (old_set) {
                        home->bitmap &= ~old_bit;
                        home->sz--;
                    }
                    // performing the trade
                    std::swap(candidate->key, bucket->key);
                    std::swap(candidate->val, bucket->val);
                    std::swap(candidate->home_idx, bucket->home_idx);
                    // updating pos and diff because of possible next iteration
                    pos = i;
                    bucket = &b[pos];
                    diff = (pos - og_idx) & local_mask;
                    moved = true;
                    break;
                }
            }
            if (!moved) { // full table, rehash
                rehash();
                return;
            }
        }
        HopBucket<T, T_r>* origin = &b[og_idx];
        const uint64_t origin_bit = 1ULL << diff;
        if ((origin->bitmap & origin_bit) == 0) { // finally updating the bitmap of original hash
            origin->bitmap |= origin_bit;
            origin->sz++; // increment size of neighbourhood
        }
        if (origin->isFull() || (count * 10 > capacity * 7)) { // After setting a bit in bitmap, check if neighbourhood is now full
            rehash();
        }
    }

    T_r* end() { return nullptr; }

    T_r* find(const T& key) {
        size_t local_mask = mask;
        const size_t idx = crc_hash(key) & local_mask;
        HopBucket<T, T_r>* bucket = &b[idx];
        if (bucket->is_occupied && bucket->key == key) {
            return &bucket->val;
        }
        if (!bucket->is_occupied && bucket->sz == 0) {
            return end();
        }
        uint64_t bits = bucket->bitmap; // iterate only set bits via ctz trick
        while (bits) {
            const unsigned int offset = static_cast<unsigned int>(__builtin_ctzll(bits)); // index of lowest set bit
            bits &= bits - 1; // clear lowest set bit
            const size_t check_idx = (idx + static_cast<size_t>(offset)) & local_mask;
            HopBucket<T, T_r>* candidate = &b[check_idx];
            if (candidate->is_occupied && candidate->key == key) {
                return &candidate->val;
            }
        }
        return end();
    }
};