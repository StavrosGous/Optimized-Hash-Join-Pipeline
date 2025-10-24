#include <vector>
#define H 64
template<typename T, typename T_r>
class HopscotchMap {
    std::vector<Bucket<T, T_r>> b;
    size_t capacity;
    private:
        void rehash() {
            std::vector<Bucket<T, T_r>> old_buckets = b;
            b.clear();
            capacity *= 2;
            b.resize(capacity);
            mask = capacity - 1;
            for (auto &old_bucket : old_buckets) {
                if (old_bucket.is_occupied) {
                    emplace(std::move(old_bucket.key), std::move(old_bucket.val));
                }
            }
        }
        void emplace(T key, T_r val) {
            size_t idx = std::hash<T>()(key) & mask; // hash of key to be inserted
            size_t og_idx = idx; // save original key hash for future reference
            while (b[idx].is_occupied) { // linear search to find empty bucket
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
            int pos = idx; // saving position of empty bucket
            int diff = (pos - og_idx) & mask; // distance of empty bucket from original key hash
            b[pos].is_occupied = true; // Claim the bucket
            b[pos].key = std::move(key);
            b[pos].val = std::move(val);
            while (diff >= H) { // if distance is at least H, need to hopscotch
                for (int i = pos-H+1; i < pos; ++i) { // Checking the previous H-1 buckets for possible trade
                    int curhash = std::hash<T>()(b[i].key) & mask; // hash of current (i'th) key
                    int tempdiff = (pos - curhash) & mask; // calculate the distance between bucket and candidate for trade
                    if (tempdiff < H) { // if distance is less than H, we trade
                        // updating bitmap representation
                        b[curhash].bitmap[pos - curhash] = true; 
                        b[curhash].bitmap[i - curhash] = false;
                        // performing the trade
                        std::swap(b[i].key, b[pos].key);
                        std::swap(b[i].val, b[pos].val);
                        // updating pos and diff because of possible next iteration
                        pos = i;
                        diff = (pos - og_idx) & mask;
                        break;
                    }
                    if (i==pos-1) { // full table, rehash
                        rehash();
                        return;
                    }
                }
            }
            b[og_idx].bitmap[pos-og_idx] = true; // finally updating the bitmap of original hash
            b[og_idx].sz++; // increment size of neighbourhood
            if (b[og_idx].isFull()) { // After setting a bit in bitmap, check if neighbourhood is now full
                rehash();
            }
        }
    };


template<typename T, typename T_r>
class Bucket {
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
    Bucket() : key(), val(), sz(0), is_occupied(false) {
        memset(bitmap, 0, sizeof(bitmap));
    }

    friend class HopscotchMap<T, T_r>;

};