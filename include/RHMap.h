#pragma once

#include <vector>
#include <ostream>
#include <utility>
#include <functional>

#define CAPACITY 16

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

    void update(T&& key, T_r&& val, const size_t &psl) {
        this->key         = std::move(key);
        this->val         = std::move(val);
        this->psl         = psl;
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
        size_t idx = std::hash<T>()(key) & mask;
        size_t psl = 0;
        while (b[idx].is_occupied) {
            if (b[idx].key == key) {
                b[idx].val = std::move(val);
                return;
            }
            if (b[idx].psl < psl) {
                std::swap(key, b[idx].key);
                std::swap(val, b[idx].val);
                std::swap(psl, b[idx].psl);
            }
            idx = (idx + 1) & mask;
            ++psl;
        }
        b[idx].update(std::move(key), std::move(val), psl);
        ++count;
    }

    T_r* end() { return nullptr; }

    T_r* find(const T& key) {
        size_t idx = std::hash<T>{}(key) & mask;
        size_t cpsl = 0;
        while (b[idx].is_occupied) {
            if (b[idx].key == key) {
                return &b[idx].val;
            }
            if (b[idx].psl < cpsl) {
                break;
            }
            idx = (idx + 1) & mask;
            ++cpsl;
        }
        return end();
    }


    size_t get_count() const { return count; }
    size_t get_capacity() const { return capacity; }

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
