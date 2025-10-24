#pragma once
#include <vector>
#define H 64

template<typename T, typename T_r>
class HopscotchMap;


template<typename T, typename T_r>
class HopBucket {
private:
    T key;
    T_r val;
    bool bitmap[H];
    int sz;
    bool is_occupied;
    bool isFull() {
        return sz == H;
    }

public:
    HopBucket() : key(), val(), sz(0), is_occupied(false) {
        memset(bitmap, 0, sizeof(bitmap));
    }

    void update(T&& key, T_r&& val) {
        this->key = std::move(key);
        this->val = std::move(val);
    }
    friend class HopscotchMap<T, T_r>;
};


template<typename T, typename T_r>
class HopscotchMap {
private:
    std::vector<HopBucket<T, T_r>> b;
    size_t capacity;
    size_t mask;
    
        void rehash() {
            std::vector<HopBucket<T, T_r>> old_Hopbuckets = b;
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

    HopscotchMap() : capacity([]() {
        size_t cap = 16;
        return cap > 0 ? 1 << (sizeof(size_t) * 8 - __builtin_clzll(cap - 1)) : 1; }()),
         mask(this->capacity - 1)
    {
        b.resize(this->capacity);
    }

    HopscotchMap(const size_t &capacity) : capacity(capacity > 0 ? 1 << (sizeof(size_t) * 8 - __builtin_clzll(capacity - 1)) : 1),
        mask(this->capacity - 1)
    {
        b.resize(this->capacity);
    }

    void emplace(T key, T_r val) {
        size_t idx = std::hash<T>()(key) & mask; // hash of key to be inserted
        size_t og_idx = idx; // save original key hash for future reference
        while (b[idx].is_occupied) { // linear search to find empty Hopbucket
            if (b[idx].key == key) {
                b[idx].val = std::move(val);
                return;
            }
            idx = (idx + 1) & mask;
            if (idx == og_idx) { // full table, need to rehash
                rehash();
                return;
            }
        }
        int pos = idx; // saving position of empty Hopbucket
        int diff = (pos - og_idx) & mask; // distance of empty Hopbucket from original key hash
        b[pos].is_occupied = true; // Claim the Hopbucket
        b[pos].key = std::move(key);
        b[pos].val = std::move(val);
        while (diff >= H) { // if distance is at least H, need to hopscotch
            for (int i = (pos - H + 1) & mask; i != pos; i = (i + 1) & mask) { // Checking the previous H-1 Hopbuckets for possible trade
                int curhash = std::hash<T>()(b[i].key) & mask; // hash of current (i'th) key
                int tempdiff = (pos - curhash) & mask; // calculate the distance between Hopbucket and candidate for trade
                if (tempdiff < H) { // if distance is less than H, we trade
                    // updating bitmap representation
                    b[curhash].bitmap[(pos - curhash) & mask] = true;
                    b[curhash].bitmap[(i - curhash) & mask] = false;
                    // performing the trade
                    std::swap(b[i].key, b[pos].key);
                    std::swap(b[i].val, b[pos].val);
                    // updating pos and diff because of possible next iteration
                    pos = i;
                    diff = (pos - og_idx) & mask;
                    break;
                }
                if (i == ((pos - 1) & mask)) { // full table, rehash
                    rehash();
                    return;
                }
            }
        }
        b[og_idx].bitmap[diff] = true; // finally updating the bitmap of original hash
        b[og_idx].sz++; // increment size of neighbourhood
        if (b[og_idx].isFull()) { // After setting a bit in bitmap, check if neighbourhood is now full
            rehash();
        }
    }

    std::pair<T, T_r&> *end() { return nullptr; }

    std::pair<T, T_r&> *find(const T& key) {
        size_t idx = std::hash<T>{}(key) & mask;
        if (!b[idx].is_occupied) {
            return end();
        }
        for (int i = 0; i < H; ++i) {
            if (b[idx].bitmap[i]) {
                size_t check_idx = (idx + i) & mask;
                if (b[check_idx].key == key) {
                    return new std::pair<T, T_r&>(b[check_idx].key, b[check_idx].val);
                }
            }
        }
        return end();
    }
};

