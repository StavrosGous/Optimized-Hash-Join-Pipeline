#include <vector>
#include <cstring>
#include <ostream>

#define CAPACITY 16

template <typename T, typename T_r>
class RHMap;

template <typename T, typename T_r>
class Bucket {
private:
    T key;
    std::vector<T_r> val;
    size_t psl;
    bool is_occupied;

public:
    Bucket(): is_occupied(false), psl(0), key((T)NULL){
        val.resize(8);
    }

    ~Bucket() {
        val.clear();
    }

    void update(const T &key, T_r &val, const size_t &psl) {
        this->key = key;
        this->val.push_back(val);
        this->psl = psl;
    }
    
    friend class RHMap<T, T_r>;

};

template <typename T, typename T_r>
class RHMap {
private:

    std::vector<Bucket<T, T_r>> b;
    size_t size;
    size_t capacity;
    size_t mask;

    void insert(T &key, T_r val, size_t psl) {
        size_t idx = std::hash<T>()(key) & mask;
        size_t original_idx = idx;
        idx = (idx + psl) & mask;
        while (b.at(idx).is_occupied) {
            if (b.at(idx).key == key) {
                b.at(idx).val.push_back(val);
                return;
            }
            if (b.at(idx).psl < psl && (std::hash<T>()(b.at(idx).key) & mask) >= original_idx) {
                std::vector<T_r> temp_vector;
                b.at(idx).psl++;
                std::swap(key, b.at(idx).key);
                temp_vector = std::move(b.at(idx).val);
                b.at(idx).val.push_back(val);
                std::swap(psl, b.at(idx).psl);
                // Maybe change this instead of recursion to continue linearly with the new key,val,psl
                insert(key, val, psl);
                return;
            }
            psl++;
            idx = (idx + 1) & mask;
            // might remove later
            if (idx == original_idx) {
                return;
            }
        }
        b.at(idx).update(key, val, psl);
        b.at(idx).is_occupied = true;
        size++;
    }

public:
    RHMap() : size(0), capacity([]() {
        size_t cap = CAPACITY;
        return cap > 0 ? 1 << (sizeof(size_t) * 8 - __builtin_clzll(cap - 1)) : 1; }()),
         mask(this->capacity - 1)
    {
        b.resize(this->capacity);
    }
    RHMap(const size_t &capacity) : size(0), 
        capacity(capacity > 0 ? 1 << (sizeof(size_t) * 8 - __builtin_clzll(capacity - 1)) : 1),
        mask(this->capacity - 1)
    {
        b.resize(this->capacity);
    }

    ~RHMap() {
        b.clear();
    }

    void emplace(T key, T_r val) {
        insert(key, val, 0);
    }

    std::pair<T, T_r> *end() {
        return (std::pair<T, T_r> *)NULL;
    }

    std::pair<T, T_r> *find(T key) {
        size_t idx = std::hash<T>()(key) & mask;
        size_t original_idx = idx;
        while (b.at(idx).is_occupied)
        {
            if (b.at(idx).key == key) {
                std::pair<T, T_r> *itr = new std::pair<T, T_r>(b.at(idx).key, b.at(idx).val);
                return itr;
            }
            idx = (idx + 1) & mask;
            if (idx == original_idx) {
                break;
            }
        }
        return end();
    }

    friend std::ostream& operator<<(std::ostream& os, const RHMap& obj) {
        os << "RHMap(size=" << obj.size << ", capacity=" << obj.capacity << ")\n";
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
