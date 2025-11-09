#pragma once
#include <vector>
#include <ostream>
#include <utility>
#include <functional>
#include <cstdint>
#include <type_traits>
#include <string>
#include <cstring>
#include "utils.h"

template <typename T, typename T_r>
class CuckooMap;

template <typename T, typename T_r>
class CBucket {
private:
    T key;
    T_r val;
    bool is_occupied;

public:
    CBucket() : key(), val(), is_occupied(false) {}

    void update(T&& key, T_r&& val) {
        this->key = std::move(key);
        this->val = std::move(val);
        this->is_occupied = true;
    }
    
    friend class CuckooMap<T, T_r>;

};

template <typename T, typename T_r>
class CuckooMap {
private:
    std::vector<CBucket<T, T_r>> b1;
    std::vector<CBucket<T, T_r>> b2;
    size_t count1;
    size_t count2;
    size_t capacity;
    size_t total;
    size_t mask;



    void rehash() {
        std::vector<CBucket<T, T_r>> old_b1 = std::move(b1);
        std::vector<CBucket<T, T_r>> old_b2 = std::move(b2);
        b1.clear();
        b2.clear();
        capacity *= 2;
        total = capacity * 2;
        b1.resize(capacity);
        b2.resize(capacity);
        mask = capacity - 1;
        count1 = 0;
        count2 = 0;
        for (auto &old_bucket : old_b1) {
            if (old_bucket.is_occupied) {
                emplace(std::move(old_bucket.key), std::move(old_bucket.val));
            }
        }
        for (auto &old_bucket : old_b2) {
            if (old_bucket.is_occupied) {
                emplace(std::move(old_bucket.key), std::move(old_bucket.val));
            }
        }
    }

public:
    CuckooMap() : count1(0), count2(0),
        capacity([]() {
            size_t cap = CAPACITY;
            size_t half = cap > 1 ? (cap >> 1) : 1;
            if (half <= 1) {
                return static_cast<size_t>(1);
            }
            return static_cast<size_t>(1ULL << (sizeof(size_t) * 8 - __builtin_clzll(half - 1)));
        }()),
        total(this->capacity * 2),
        mask(this->capacity - 1)
    {
        b1.resize(this->capacity);
        b2.resize(this->capacity);
    }

    CuckooMap(const size_t &capacity) : count1(0), count2(0),
        capacity([&capacity]() {
            size_t cap = capacity;
            size_t half = cap > 1 ? (cap >> 1) : 1;
            if (half <= 1) {
                return static_cast<size_t>(1);
            }
            return static_cast<size_t>(1ULL << (sizeof(size_t) * 8 - __builtin_clzll(half - 1)));
        }()),
        total(this->capacity * 2),
        mask(this->capacity - 1)
    {
        b1.resize(this->capacity);
        b2.resize(this->capacity);
    }

    

    void emplace(T key, T_r val) {
        // if combined sizes for the 2 tables exceed load factor, rehash
        if ((count1 + count2) >= total * LOAD_FACTOR) {
            rehash();
        }

        size_t local_mask = mask;
        // calculate remaining kicks allowed
        size_t kicks_remaining = count1 + count2 + 1;
        bool place_in_first  = true;

        while (true) {
            /// according to the current table working use its values
            auto& table = place_in_first ? b1 : b2;
            auto& table_count = place_in_first ? count1 : count2;
            size_t index = (place_in_first ? crc_hash(key) : std::hash<T>{}(key)) & local_mask;
            auto& bucket = table[index];

            if (!bucket.is_occupied) {
                bucket.update(std::move(key), std::move(val));
                ++table_count;
                return;
            }
            // kick the current element out
            std::swap(key, bucket.key);
            std::swap(val, bucket.val);
            place_in_first = !place_in_first;
            // if no kicks remaining the 2 tables are full and we must rehash
            if (--kicks_remaining == 0) {
                rehash();
                kicks_remaining = count1 + count2 + 1;
                place_in_first  = true;
            }
        }
    }

    T_r* end() { return nullptr; }

    T_r* find(const T& key) {
        size_t local_mask = mask;
        size_t idx1 = crc_hash(key) & local_mask;
        CBucket<T, T_r> *bucket1 = &b1[idx1];
        if (bucket1->is_occupied && bucket1->key == key) {
            return &bucket1->val;
        }

        size_t idx2 = std::hash<T>{}(key) & local_mask;
        CBucket<T, T_r> *bucket2 = &b2[idx2];
        if (bucket2->is_occupied && bucket2->key == key) {
            return &bucket2->val;
        }

        return end();
    }

};