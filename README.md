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
Added the structs used and some helper functions to "utils.h"
- **`value_t`**: Struct for late materialization 
- **`column_t`**: Struct that contains all values of a column in multiple buffers 
(in buffer_t struct)
- **`int32_wrapper`**: 64-bit struct that uses the upper 32 bits for the value and the lower 32 bits for null info.
- **`buffer_t`**: Struct that stores a fixed amount of value_t values in an array 
- **`read_u16` / `read_i32`**: Memory read helper functions.

Added `include/unchained_ht.h` which contains the implementation of our specialized `UnchainedHashTable`. The code for the hashtable implementation followed the given paper's instructions closely. We created an "Entry" struct with 64 bits size, so that we map keys to the buffer ids and offsets the values are stored at (a similar class "T" was proposed in the paper). It is worth noting that instead of a "produce(cur)" function scheme as proposed in the paper we decided to return two Entry* pointers, one to the start of the list of duplicates returned from lookup and one to the end, so that the lookup's result can be iterated with an Entry* pointer. We also precomputed and hardcoded the tags with 4 set bits (instead of computing 1820 tags in runtime and padding them when building the hashtable).


### execute.cpp
We edited `execute.cpp` for every step of the exercise. For the late materialization step, we created a `value_t` struct as said in the exercise description. This `value_t` contains a union of `int32_wrapper` and `string_struct`. This replaces the `std::variant` based `Data` type, reducing memory overhead and avoiding `std::visit` call.

## Key changes implemented:
- **Late Materialization**: Strings are not fully materialized during the join. Instead, we store a `string_struct` containing `table_id`, `col_id`, `page_id`, and `offset`. The actual string data is retrieved from the pages only during the final materialization phase. The function `materialize_columnar_table` is responsible for the materialization of all strings. 
- **Column Store Optimization**: `ExecuteResult` is now `std::vector<column_t>`, where each `column_t` represents a column of data. And all values of a column are stored into `buffer_t` structs (column_t structs contain vectors of `value_t`).
- **Unchained Hash Table Optimization**
In the final optimization step, we replaced the hopscotch hashmap with our `UnchainedHashTable` hashmap (implemented in "/include/unchained_ht.cpp"). It should be noted, along with the previous comments on the hashmap, that unlike the old hashmaps that had an insert function, this hashmap has a build function (like in the paper but for 1 partition) that takes a column_t object as argument and then builds the hashtable on the given column.


All optimizations were implemented incrementally, with each step validated for correctness and performance improvements.