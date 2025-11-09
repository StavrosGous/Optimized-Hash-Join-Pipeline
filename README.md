
![Actions Dashboard](https://github.com/uoa-k23a/k23a-2025-d1-2-odds-1-pipeline/actions/workflows/tests-workflow.yml/badge.svg)

To run the program on plans.json do
```bash
$ ./build.sh fast_plans <NUM>
```
Where NUM corresponds to the hashmap you want to run the program with (default is Robinhood):
- **Robinhood** num: **0**
- **Hopscotch** num: **1**
- **Cuckoo** num: **2**

For example, if you want to run the program with the **Robinhood** hashmap, you run:

```bash
$ ./build.sh fast_plans 0
$ ./run.sh fast_plans
```
## Tests
For each hashmap implementation we created unit tests to ensure all methods work correctly.
These files are located in the tests folder and are used supplementary to the original tests.

To run these tests, you can use our `build.sh` and `run.sh` scripts by running the following instructions:

```bash
$ ./build.sh tests
$ ./run.sh tests
```
This will execute all of the tests. 
## Changes made

### Header files
We created 3 different header files that contain the implementations of the different hashmaps. Namely we created:
- **RHMap.h**, the **Robinhood** hashmap
- **HopscotchMap.h**, the **Hopscotch** hashmap
- **CuckooMap.h**, the **Cuckoo** hashmap.

We also created a utils.h file that contains the crc32 and splitmix functions.

### execute.cpp
We modified `execute.cpp` a bit as we changed the interfaces of `find()` and `end()` methods of the hashmaps, so they dont return `iterator` but the `vector` of values.

## Implementation
We followed as closely as possible the instructions of the assignment's description for the implementation of all algorithms. For optimization purposes we use [`crc_hash`](https://github.com/sigmod25-teamOPT/optimized-join-pipeline/blob/main/src/unchained_table.cpp) possible collisions in both `Robinhood` and `Hopscotch`. For the Cuckoo hashmap which needs two hash functions, we used [`crc_hash`](https://github.com/sigmod25-teamOPT/optimized-join-pipeline/blob/main/src/unchained_table.cpp) as the first hash function and `std::hash` as the second.
While we made it so that our hashmaps are generic, meaning that they implement a load-factor based rehash, all hashmaps allocate capacity equal to the closest power of 2 higher than the capacity given during construction, which is 2 times the `build_size` calculated by the size of `left` `ExecuteResult` if the `build_left` flag is set or with size of `right` otherwise. Therefore, when the information about capacity is available, we can construct a big enough hashmap from the start so that the rehash function will never be called. Specifically, by allocating the array so that in the worst case it has a 0.625 load factor, the hashmap will be only 62.5% filled after inserting all entries (if there are duplicate entries then the load factor will be lower than 62.5%). 




