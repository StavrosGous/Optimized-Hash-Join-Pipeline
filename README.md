![Actions Dashboard](https://github.com/uoa-k23a/k23a-2025-d1-2-odds-1-pipeline/actions/workflows/tests-workflow.yml/badge.svg)
![LeaderBoard Run](https://github.com/uoa-k23a/k23a-2025-d1-2-odds-1-pipeline/actions/workflows/leaderboard.yml/badge.svg)

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
Added the structs used and some helper functions to "utils.h":
- **`column_t`**: Struct that contains all values of a column in multiple buffers 
- **`buffer_t`**: Struct that stores a fixed amount of value_t values in an array 
- **`value_t`**: 64-bit union of int32_wrapper and string_struct for late materialization
(in buffer_t struct)
- **`string_struct`**: 64-bit struct that stores the table_id, the column_id, the page_id and the offset inside the page
for materializing the string at the end
- **`int32_wrapper`**: 64-bit struct that uses the upper 32 bits for the value and the lower 32 bits for null info.
- **`read_u16` & `read_i32`**: Memory read helper functions.

### execute.cpp
We edited `execute.cpp` for every step of the exercise. For the late materialization step, we created a `value_t` struct as said in the exercise description. This `value_t` contains a union of `int32_wrapper` and `string_struct`. This replaces the `std::variant` based `Data` type, reducing memory overhead and avoiding `std::visit` call.

## Key changes implemented:
- **Late Materialization**: Strings are not fully materialized during the join. Instead, we store a `string_struct`. The actual string data is retrieved from the pages only during the final materialization phase. The function `materialize_columnar_table` is responsible for the materialization of all strings. 
- **Column Store Optimization**: `ExecuteResult` is now `std::vector<column_t>`, where each `column_t` represents a column of data. And all values of a column are stored into `buffer_t` structs (column_t structs contain vectors of `value_t`).
- **Unchained Hash Table Optimization**
We replaced the hopscotch hashmap with our `UnchainedHashTable` hashmap, which is implemented in `include/unchained_ht.h`. Our implementation followed the given paper's instructions closely. We created an "Entry" struct with 64 bits size, so that we map keys to the buffer ids and offsets the values are stored at (a similar class "T" was proposed in the paper). It is worth noting that instead of a "produceMatches" function scheme as proposed in the paper we decided to return two Entry* pointers, one to the start of the list of duplicates returned from lookup and one to the end, so that the lookup's result can be iterated with an Entry* pointer. We also precomputed and hardcoded the 1820 tags with 4 set bits and padded them uniformly to 2048 (instead of making such computations in runtime). Furthermore unlike the old hashmaps that had an insert function, this hashmap has a build function (as in the paper but for 1 partition) that takes a column_t object as argument and then builds the hashtable on the given column.


All optimizations were implemented incrementally, with each step validated for correctness and performance improvements.
