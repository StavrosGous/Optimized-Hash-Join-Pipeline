![Actions Dashboard](https://github.com/uoa-k23a/k23a-2025-d1-2-odds-1-pipeline/actions/workflows/tests-workflow.yml/badge.svg)

To build and run the program on plans.json do
```bash
$ ./build.sh fast_plans
$ ./run.sh fast_plans
```
## Tests

To run these tests, you can use our `build.sh` and `run.sh` scripts by running the following instructions:

```bash
$ ./build.sh tests
$ ./run.sh tests
```

This will execute all of the tests. 
## Changes made

### Header files
We expanded `include/utils.h` significantly. Initially containing only hashing functions (`crc_hash`), it now houses the core data structures for our columnar engine:
- **`value_t`**: The unified data type for late materialization.
- **`buffer_t` & `column_t`**: Structures for managing columnar data chunks.
- **`read_u16` / `read_i32`**: Helpers for unaligned memory reads.

Additionally, we added `include/unchained_ht.h` which contains the implementation of our specialized `UnchainedHashTable`. This header defines the `Entry` structure and the hash table logic optimized for our columnar data format.

### execute.cpp
We edited `execute.cpp` for every step of the exercise. For the late materialization step, we created a `value_t` struct as said in the exercise description. This `value_t` contains a union of `int32_t` and `string_struct`, along with bitfields for `is_string` and `is_null` flags. This replaces the `std::variant` based `Data` type, reducing memory overhead and avoiding `std::visit` calls.

```cpp
struct value_t {
    // pack flags into a single byte using bitfields to reduce size and improve cache
    unsigned is_string : 1;
    unsigned is_null : 1;
    union {
        int32_t       int32_val;
        string_struct str_val;
    };
};
```

Key changes implemented:
- **Late Materialization**: Strings are not fully materialized during the join. Instead, we store a `string_struct` containing `table_id`, `col_id`, `page_id`, and `offset`. The actual string data is retrieved from the pages only during the final materialization phase.
- **Custom Value Type**: The `value_t` struct is packed to minimize size and improve cache locality.
- **Manual Page Parsing**: We implemented `build_column_inserter` to manually read values and null bitmaps directly from raw page data, bypassing higher-level iterator overhead.
- **Direct Type Access**: The join algorithm now accesses values directly via `value_t` members, eliminating the runtime overhead of `std::visit`.

### Column Store Optimization
For the second step, we transitioned from a row-oriented execution model to a column-oriented one.
- **Columnar Layout**: `ExecuteResult` is now `std::vector<column_t>`, where each `column_t` represents a column of data.
- **Buffered Storage**: Data within columns is organized into `buffer_t` chunks (vectors of `value_t`). This improves cache locality and allows for processing data in blocks.
- **Struct Changes**: `value_t` was simplified to a union of `int32_wrapper` and `string_struct`. `int32_wrapper` includes a `status` field to handle NULL values, removing the need for separate bitflags in the main struct.
- **Vectorized Processing**: The join algorithm and scan operators were updated to iterate over these buffers, enabling better compiler optimizations and potentially auto-vectorization.

```cpp
struct buffer_t {
    value_t* data;
    uint16_t count;
    // ... (constructors and destructors)
};

struct column_t {
    std::vector<buffer_t> buffers;
    size_t                num_rows;
    DataType              type;
    // ...
};
```

### Unchained Hash Table Optimization
In the final optimization step, we replaced the general-purpose hash maps with a specialized `UnchainedHashTable` tailored for our columnar data layout.
- **Specialized Design**: The hash table is built directly from `column_t` buffers, minimizing data movement and transformation overhead.
- **Directory-based Structure**: It uses a large directory array to index into a dense array of `Entry` structs, effectively creating a perfect hash table for the directory slots.
- **Tagging for Filtering**: We incorporated 11-bit tags within the directory entries. These tags allow us to check for potential key matches before accessing the actual data, significantly reducing unnecessary cache misses during lookups.
- **Software Prefetching**: We added `__builtin_prefetch` instructions to preload directory entries into the cache during the probe phase, hiding memory access latency.
- **Efficient Duplicate Handling**: The `lookup` method returns a range (start and end pointers) of matching entries, streamlining the handling of join keys with multiple matches.

```cpp
class UnchainedHashTable {
public:
    struct Entry {
        int32_t key;
        uint16_t buf_idx;
        uint16_t offset;
    };

    UnchainedHashTable(size_t sz);
    ~UnchainedHashTable();

    void build(const column_t& col);
    void prefetch(const int32_t key) const;
    std::pair<Entry*, Entry*> lookup(const int32_t key) const;
};
```

## Implementation
We followed the exercise description and the recommendations from the referenced research paper for implementing all algorithms. Specifically:

- **Hash Function**: We retained the `crc_hash` function for its efficiency and low collision rate. This was used in both `Robinhood` and `Hopscotch` hashmaps. For the `Cuckoo` hashmap, we combined `crc_hash` with `std::hash` to meet the requirement for two hash functions.
- **Hashmap Design**: Our hashmaps are generic and implement load-factor-based rehashing. However, we optimized capacity allocation to avoid rehashing during execution. By allocating arrays with a worst-case load factor of 0.625, we ensured efficient memory usage and performance.
- **Unchained Hash Table**: For the unchained hash table, we closely followed the directory/tagging structure and range-based lookup design described in the recommended paper. This included optimizations for cache locality and efficient duplicate handling.
- **execute.cpp Choices**: In `execute.cpp`, we made deliberate choices to align with best practices in analytical database systems:
  - **Late Materialization**: Strings are stored as references (`string_struct`) and only materialized during the final phase.
  - **Columnar Processing**: Data is processed in columnar format using `buffer_t` and `column_t` structures to maximize cache locality.
  - **Direct Memory Access**: We bypassed higher-level abstractions to directly access raw page data, reducing overhead.

All optimizations were implemented incrementally, with each step validated for correctness and performance improvements.




