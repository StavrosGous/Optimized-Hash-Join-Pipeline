#pragma once
#include <vector>
#include <ostream>
#include <utility>
#include <functional>
#include <cstdint>
#include <type_traits>
#include <string>
#include <cstring>

#define CAPACITY 16
#define LOAD_FACTOR 0.5

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
    size_t mask;


    
    static uint64_t murmur_hash_64(const void* key, int len, uint64_t seed) {
        constexpr uint64_t m = 0xc6a4a7935bd1e995ULL;
        constexpr int      r = 47;

        uint64_t h = seed ^ (static_cast<uint64_t>(len) * m);

        const auto* data = static_cast<const unsigned char*>(key);
        const auto* end  = data + (len & ~7);

    while (data != end) {
            uint64_t k;
            std::memcpy(&k, data, sizeof(uint64_t));
            data += sizeof(uint64_t);
#if defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
            k = __builtin_bswap64(k);
#endif
            k *= m;
            k ^= k >> r;
            k *= m;

            h ^= k;
            h *= m;
        }

    const auto* tail = data;

    switch (len & 7) {
    case 7: h ^= static_cast<uint64_t>(tail[6]) << 48; [[fallthrough]];
    case 6: h ^= static_cast<uint64_t>(tail[5]) << 40; [[fallthrough]];
    case 5: h ^= static_cast<uint64_t>(tail[4]) << 32; [[fallthrough]];
    case 4: h ^= static_cast<uint64_t>(tail[3]) << 24; [[fallthrough]];
    case 3: h ^= static_cast<uint64_t>(tail[2]) << 16; [[fallthrough]];
    case 2: h ^= static_cast<uint64_t>(tail[1]) << 8;  [[fallthrough]];
    case 1: h ^= static_cast<uint64_t>(tail[0]);
                h *= m;
        }

        h ^= h >> r;
        h *= m;
        h ^= h >> r;

        return h;
    }

    size_t murmur_hash(const T& key) const {
        const uint64_t seed = 0x9e3779b97f4a7c15ULL;
        if constexpr (std::is_arithmetic_v<T>) {
            return static_cast<size_t>(murmur_hash_64(&key, sizeof(T), seed));
        } else if constexpr (std::is_same_v<T, std::string>) {
            return static_cast<size_t>(murmur_hash_64(key.data(), static_cast<int>(key.size()), seed));
        } else {
            auto hashed = static_cast<uint64_t>(std::hash<T>{}(key));
            return static_cast<size_t>(murmur_hash_64(&hashed, sizeof(hashed), seed));
        }
        // return std::hash<T>{}(key);
    }

    void rehash() {
        std::vector<CBucket<T, T_r>> old_b1 = b1;
        std::vector<CBucket<T, T_r>> old_b2 = b2;
        b1.clear();
        b2.clear();
        capacity *= 2;
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
    CuckooMap() : count1(0), count2(0), capacity([]() {
        size_t cap = CAPACITY;
        return cap > 0 ? 1 << (sizeof(size_t) * 8 - __builtin_clzll(cap - 1)) : 1; }()),
        mask(this->capacity - 1)
    {
        b1.resize(this->capacity);
        b2.resize(this->capacity);
    }

    CuckooMap(const size_t &capacity) : count1(0), count2(0),
        capacity(capacity > 0 ? 1 << (sizeof(size_t) * 8 - __builtin_clzll(capacity - 1)) : 1),
        mask(this->capacity - 1)
    {
        b1.resize(this->capacity);
        b2.resize(this->capacity);
    }

    

    void emplace(T key, T_r val) {
        // if combined sizes fo the 2 tables exceed load factor, rehash
        if ((count1 + count2) >= static_cast<size_t>(capacity * LOAD_FACTOR)) {
            rehash();
        }

        // calculate remaining kicks allowed
        size_t kicks_remaining = capacity * 2 + 1;
        bool   place_in_first  = true;

        while (true) {
            /// according to the current table working use its values
            auto&  table       = place_in_first ? b1 : b2;
            auto&  table_count = place_in_first ? count1 : count2;
            size_t index       = (place_in_first ? std::hash<T>{}(key) : murmur_hash(key)) & mask;
            auto&  bucket      = table[index];

            if (!bucket.is_occupied) {
                bucket.update(std::move(key), std::move(val));
                ++table_count;
                return;
            }

            if (bucket.key == key) {
                bucket.val = std::move(val);
                return;
            }
            // kick the current element out
            std::swap(key, bucket.key);
            std::swap(val, bucket.val);
            place_in_first = !place_in_first;
            // if no kicks remaining the 2 tables are full and we must rehash
            if (--kicks_remaining == 0) {
                rehash();
                emplace(std::move(key), std::move(val));
                return;
            }
        }
    }

    T_r* end() { return nullptr; }

    T_r* find(const T& key) {
        size_t index1 = std::hash<T>{}(key) & mask;
        auto&  bucket1 = b1[index1];
        if (bucket1.is_occupied && bucket1.key == key) {
            return &bucket1.val;
        }

        size_t index2 = murmur_hash(key) & mask;
        auto&  bucket2 = b2[index2];
        if (bucket2.is_occupied && bucket2.key == key) {
            return &bucket2.val;
        }

        return end();
    }


    size_t get_capacity() const { return capacity; }

    friend std::ostream& operator<<(std::ostream& os, const CuckooMap& obj) {
        os << "CuckooMap(size1=" << obj.count1 << ", size2=" << obj.count2 << ", capacity=" << obj.capacity << ")\n";
        os << "Table 1:\n";
        for (size_t i = 0; i < obj.b1.size(); ++i) {
            const auto& bucket = obj.b1[i];
            os << "[" << i << "] ";
            if (!bucket.is_occupied) {
                os << "<empty>";
            } else {
                os << "key=" << bucket.key << ", vals=[";
                for (size_t j = 0; j < bucket.val.size(); ++j) {
                    os << bucket.val[j];
                    if (j + 1 < bucket.val.size()) os << ", ";
                }
                os << "]";
            }
            os << "\n";
        }
        os << "Table 2:\n";
        for (size_t i = 0; i < obj.b2.size(); ++i) {
            const auto& bucket = obj.b2[i];
            os << "[" << i << "] ";
            if (!bucket.is_occupied) {
                os << "<empty>";
            } else {
                os << "key=" << bucket.key << ", vals=[";
                for (size_t j = 0; j < bucket.val.size(); ++j) {
                    os << bucket.val[j];
                    if (j + 1 < bucket.val.size()) os << ", ";
                }
                os << "]";
            }
            os << "\n";
        }
        return os;
    }
};
