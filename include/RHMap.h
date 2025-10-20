#include <vector>
#include <cstring>
#define CAPACITY 16

template <typename T, typename T_r>
class RHMap;

template <typename T, typename T_r>
class Bucket
{
private:
    T key;
    std::vector<T_r> *val;
    size_t psl;
    bool is_occupied;

public:
    Bucket()
    {
        is_occupied = false;
        psl = 0;
        key = (T)NULL;
        val = new std::vector<T_r>(8);
    };
    ~Bucket()
    {
        if (val != (T_r)NULL)
        {
            delete val;
        }
    }
    void update(const T &key, T_r &val, const size_t &psl)
    {
        this->key = key;
        if (this->val.empty)
        {
            delete this->val;
        }
        this->val = std::move(val);
        this->psl = psl;
    }
    friend class RHMap<T, T_r>;
};

template <typename T, typename T_r>
class RHMap
{
private:

    std::vector<Bucket<T, T_r>> b;
    size_t size;
    size_t capacity;
    size_t mask;

    void insert(T &key, T_r val, size_t psl)
    {
        size_t idx = std::hash<T>()(key) & mask;
        size_t original_idx = idx;
        idx = (idx + psl) & mask;
        while (b[idx].is_occupied)
        {
            if (b[idx].key == key)
            {
                b[idx].val->push_back(val);
                return;
            }
            if (b[idx].psl < psl && (std::hash<T>()(b[idx].key) & mask) >= original_idx)
            {
                b[idx].psl++;
                std::swap(key, b[idx].key);
                std::swap(val, b[idx].val);
                std::swap(psl, b[idx].psl);
                // Maybe change this instead of recursion to continue linearly with the new key,val,psl
                insert(key, val, psl);
                return;
            }
            psl++;
            idx = (idx + 1) & mask;
            // might remove later
            if (idx == original_idx)
            {
                return;
            }
        }
        b[idx].update(key, val, psl);
        b[idx].is_occupied = true;
        size++;
    }

public:
    RHMap() : size(0), capacity([]() {
        size_t cap = CAPACITY;
        return cap > 0 ? 1 << (sizeof(size_t) * 8 - __builtin_clzll(cap - 1)) : 1; }()), mask(this->capacity - 1)
    {
        b.resize(this->capacity);
    }
    RHMap(const size_t &capacity) : size(0), capacity(capacity > 0 ? 1 << (sizeof(size_t) * 8 - __builtin_clzll(capacity - 1)) : 1), mask(this->capacity - 1)
    {
        b.resize(this->capacity);
    }
    ~RHMap()
    {
        b.clear();
    }

    void emplace(T key, T_r val)
    {
        insert(key, val, 0);
    }

    std::pair<T, T_r> *end()
    {
        return (std::pair<T, T_r> *)NULL;
    }

    std::pair<T, T_r> *find(T key)
    {
        size_t idx = std::hash<T>()(key) & mask;
        size_t original_idx = idx;
        while (b[idx].is_occupied)
        {
            if (b[idx].key == key)
            {
                std::pair<T, T_r> *itr = new std::pair<T, T_r>(b[idx].key, b[idx].val);
                return itr;
            }
            idx = (idx + 1) & mask;
            if (idx == original_idx)
            {
                break;
            }
        }
        return end();
    }
};
