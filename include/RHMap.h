#include <vector>
#include <cstring>
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
        Bucket() {
            is_occupied = false;
            psl = 0;
            key = (T) NULL;
            val = (T_r) NULL;
        };
        void update(const T &key, const T_r &val, const size_t psl) {
            this->key = key;
            this->val = val;
            this->psl = psl;
        }
        friend class RHMap<T, T_r>;
              
};

template <typename T, typename T_r>
class RHMap{
    private:
        Bucket<T, T_r> b[CAPACITY]; // Might initialize it with new later in constructor
        size_t size;
        void insert(T &key, T_r &val, size_t psl) {
            size_t idx = std::hash<T>()(key ) % CAPACITY;
            size_t original_idx = idx;
            idx = (idx + psl) % CAPACITY;
            while (b[idx].is_occupied) {
                if (b[idx].psl < psl && (std::hash<T>()(b[idx].key) % CAPACITY) >= original_idx) {
                    b[idx].psl++;
                    std::swap(key, b[idx].key);
                    std::swap(val, b[idx].val);
                    std::swap(psl, b[idx].psl);
                    insert(key, val, psl);
                    return;
                }
                psl++;
                idx = (idx + 1) % CAPACITY;
                if (idx == original_idx) {
                    std::cout<< "Insert failed" << std::endl;
                    return;
                }
            }
            b[idx].update(key, val, psl);
            b[idx].is_occupied = true;
            size++;
        }

    public:
        RHMap() {
            size = 0;
            memset(b, 0, sizeof(b));
        }
        void insert(T key, T_r val) {
            insert(key, val, 0);
        }

        
};
