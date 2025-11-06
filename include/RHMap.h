#pragma once

#include <cstdint>
#include <functional>
#include <ostream>
#include <utility>
#include <vector>

#define CAPACITY 16

inline std::uint64_t splitmix64(std::uint64_t x) {
    x += 0x9e3779b97f4a7c15ULL;
    x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9ULL;
    x = (x ^ (x >> 27)) * 0x94d049bb133111ebULL;
    return x ^ (x >> 31);
}

template <typename T, typename T_r>
class RHMap;

template <typename T, typename T_r>
class Bucket {
private:
    T key;
    T_r val;
    size_t psl;
    bool is_occupied;

public:
    Bucket() : key(), val(), psl(0), is_occupied(false) {}

    void update(T&& key, T_r&& val, size_t psl_val) {
        this->key = std::move(key);
        this->val = std::move(val);
        this->psl = psl_val;
        this->is_occupied = true;
    }
    
    friend class RHMap<T, T_r>;

};

template <typename T, typename T_r>
class RHMap {
private:
    std::vector<Bucket<T, T_r>> b;
    size_t count;
    size_t capacity;
    size_t mask;

    
public:
    RHMap() : count(0), capacity([]() {
        size_t cap = CAPACITY;
        return cap > 0 ? 1 << (sizeof(size_t) * 8 - __builtin_clzll(cap - 1)) : 1; }()),
         mask(this->capacity - 1)
    {
        b.resize(this->capacity);
    }
    RHMap(const size_t &capacity) : count(0),
        capacity(capacity > 0 ? 1 << (sizeof(size_t) * 8 - __builtin_clzll(capacity - 1)) : 1),
        mask(this->capacity - 1)
    {
        b.resize(this->capacity);
    }

    void emplace(T key, T_r val) {
        auto* buckets = b.data();
        const size_t local_mask = mask;
        size_t idx = splitmix64(static_cast<std::uint64_t>(std::hash<T>{}(key))) & local_mask;
        size_t psl = 0;

        while (true) {
            auto& bucket = buckets[idx];
            if (!bucket.is_occupied) {
                bucket.update(std::move(key), std::move(val), psl);
                ++count;
                return;
            }
            if (bucket.psl < psl) {
                std::swap(key, bucket.key);
                std::swap(val, bucket.val);
                std::swap(psl, bucket.psl);
            }
            idx = (idx + 1) & local_mask;
            ++psl;
        }
    }

    T_r* end() { return nullptr; }

    T_r* find(const T& key) {
        auto* buckets = b.data();
        const size_t local_mask = mask;
        size_t idx = splitmix64(static_cast<std::uint64_t>(std::hash<T>{}(key))) & local_mask;
        size_t cpsl = 0;

        while (true) {
            auto& bucket = buckets[idx];
            if (!bucket.is_occupied) {
                return end();
            }
            if (bucket.key == key) {
                return &bucket.val;
            }
            if (bucket.psl < cpsl) {
                break;
            }
            idx = (idx + 1) & local_mask;
            ++cpsl;
        }
        return end();
    }



    friend std::ostream& operator<<(std::ostream& os, const RHMap& obj) {
        os << "RHMap(size=" << obj.count << ", capacity=" << obj.capacity << ")\n";
        for (size_t i = 0; i < obj.b.size(); ++i) {
            const auto& bucket = obj.b[i];
            os << "[" << i << "] ";
            if (!bucket.is_occupied) {
                os << "<empty>";
            } else {
                os << "key=" << bucket.key << ", psl=" << bucket.psl << ", vals=[";
                for (size_t j = 0; j < bucket.val.size(); ++j) {
                    os << bucket.val[j];
                    if (j + 1 < bucket.val.size()) os << ", ";
                }
                os << "]";
            }
            if (i + 1 < obj.b.size()) os << '\n';
        }
        return os;
    }
};