[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/gjaw_qSU)
# SIGMOD Contest 2025

## Task

Given the joining pipeline and the pre-filtered input data, your task is to implement an efficient joining algorithm to accelerate the execution time of the joining pipeline. Specifically, you need to implement the following function in `src/execute.cpp`:

```C++
ColumnarTable execute(const Plan& plan, void* context);
```

Optionally, you can implement these two functions as well to prepare any global context (e.g., thread pool) to accelerate the execution.

```C++
void* build_context();
void destroy_context(void*);
```

### Input format

The input plan in the above function is defined as the following struct.

```C++
struct ScanNode {
    size_t base_table_id;
};

struct JoinNode {
    bool   build_left;
    size_t left;
    size_t right;
    size_t left_attr;
    size_t right_attr;
};

struct PlanNode {
    std::variant<ScanNode, JoinNode>          data;
    std::vector<std::tuple<size_t, DataType>> output_attrs;
};

struct Plan {
    std::vector<PlanNode>      nodes;
    std::vector<ColumnarTable> inputs;
    size_t root;
}
```

**Scan**:
- The `base_table_id` member refers to which input table in the `inputs` member of a plan is used by the Scan node.
- Each item in the `output_attrs` indicates which column in the base table should be output and what type it is.

**Join**:
- The `build_left` member refers to which side the hash table should be built on, where `true` indicates building the hash table on the left child, and `false` indicates the opposite.
- The `left` and `right` members are the indexes of the left and right child of the Join node in the `nodes` member of a plan, respectively.
- The `left_attr` and `right_attr` members are the join condition of Join node. Supposing that there are two records, `left_record` and `right_record`, from the intermediate results of the left and right child, respectively. The members indicate that the two records should be joined when `left_record[left_attr] == right_record[right_attr]`.
- Each item in the `output_attrs` indicates which column in the result of children should be output and what type it is. Supposing that the left child has $n_l$ columns and the right child has $n_r$ columns, the value of the index $i \in \{0, \dots, n_l + n_r - 1\}$, where the ranges $\{0, \dots, n_l - 1\}$ and $\{n_l, \dots, n_l + n_r - 1\}$ indicate the output column is from left and right child respectively.

**Root**: The `root` member of a plan indicates which node is the root node of the execution plan tree.

### Data format

The input and output data both follow a simple columnar data format.

```C++
enum class DataType {
    INT32,       // 4-byte integer
    INT64,       // 8-byte integer
    FP64,        // 8-byte floating point
    VARCHAR,     // string of arbitary length
};

constexpr size_t PAGE_SIZE = 8192;

struct alignas(8) Page {
    std::byte data[PAGE_SIZE];
};

struct Column {
    DataType           type;
    std::vector<Page*> pages;
};

struct ColumnarTable {
    size_t              num_rows;
    std::vector<Column> columns;
};
```

A `ColumnarTable` first stores how many rows the table has in the `num_rows` member, then stores each column seperately as a `Column`. Each `Column` has a type and stores the items of the column into several pages. Each page is of 8192 bytes. In each page:

- The first 2 bytes are a `uint16_t` which is the number of rows $n_r$ in the page.
- The following 2 bytes are a `uint16_t` which is the number of non-`NULL` values $n_v$ in the page.
- The first $n_r$ bits in the last $\left\lfloor\frac{(n_r + 7)}{8}\right\rfloor$ bytes is a bitmap indicating whether the corresponding row has value or is `NULL`.

**Fixed-length attribute**: There are $n_v$ contiguous values begins at the first aligned position. For example, in a `Page` of `INT32`, the first value is at `data + 4`. While in a `Page` of `INT64` and `FP64`, the first value is at `data + 8`.

**Variable-length attribute**: There are $n_v$ contigous offsets (`uint16_t`) begins at `data + 4` in a `Page`, followed by the content of the varchars which begins at `char_begin = data + 4 + n_r * 2`. Each offset indicates the ending offset of the corresponding `VARCHAR` with respect to the `char_begin`.

**Long string**: When the length of a string is longer than `PAGE_SIZE - 7`, it can not fit in a normal page. Special pages will be used to store such string. If $n_r$ `== 0xffff` or $n_r$ `== 0xfffe`, the `Page` is a special page for long string. `0xffff` means the page is the first page of a long string and `0xfffe` means the page is the following page of a long string. The following 2 bytes is a `uint16_t` indicating the number of chars in the page, beginning at `data + 4`.

## Requirement

- You can only modify the file `src/execute.cpp` in the project.
- You must not use any third-party libraries. If you are using libraries for development (e.g., for logging), ensure to remove them before the final submission.
- The joining pipeline (including order and build side) is optimized by PostgreSQL for `Hash Join` only. However, in the `execute` function, you are free to use other algorithms and change the pipeline, as long as the result is equivalent.
- For any struct listed above, all of there members are public. You can manipulate them in free functions as desired as long as the original files are not changed and the manipulated objects can be destructed properly.
- Your program will be evaluated on an unpublished benchmark sampled from the original JOB benchmark. You will not be able to access the test benchmark.

## Quick start

> [!TIP]
> Run all the following commands in the root directory of this project.

First, download the imdb dataset.

```bash
./download_imdb.sh
```

Second, build the project.

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -Wno-dev
cmake --build build -- -j $(nproc)
```

Third, prepare the DuckDB database for correctness checking.

```bash
./build/build_database imdb.db
```

Now, you can run the tests:
```bash
./build/run plans.json
```
> [!TIP]
> If you want to use `Ninja Multi-Config` as the generator. The commands will look like:
> 
>```bash
> cmake -S . -B build -Wno-dev -G "Ninja Multi-Config"
> cmake --build build --config Release -- -j $(nproc)
> ./build/Release/build_database imdb.db
> ./build/Release/run plans.json
> ```

# Cache
## This section is only for UNIX users
There are 2 new executables with this repository. They cache the join tables and
result of each query and mmap them for faster loading times and getting rid of duckdb.

To build the cache you need to run:
```bash
./build/build_cache plans.json
```

> [!TIP] 
> If you are using `Linux x86_64` you can download our prebuilt cache with:
> ```
> wget http://share.uoa.gr/protected/all-download/sigmod25/sigmod25_cache_x86.tar.gz
> ```
> If you are using `macOS arm64` you can download our prebuilt cache with:
> ```
> wget http://share.uoa.gr/protected/all-download/sigmod25/sigmod25_cache_arm.tar.gz
> ```
> For all other systems you will need to build the cache on your own.

After the cache is built you can run the queries using:
```bash
./build/fast plans.json
```

Also after you have built the cache you no longer need to build the `run` executable
every time (which depends on duckdb and can be slow to compile). Just compile 
the executable that uses the cache:
```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -Wno-dev
cmake --build build -- -j $(nproc) fast
```

Code is compiled with Clang 18.





[Build](https://github.com/uoa-k23a/k23a-2025-d1-2-odds-1-pipeline/actions/workflows/cmake-single-platform.yml)

## std::unordered_map run plans.json
```bash
Query 1a >>              Runtime: 375 ms - Result correct: true
Query 1b >>              Runtime: 244 ms - Result correct: true
Query 1c >>              Runtime: 128 ms - Result correct: true
Query 1d >>              Runtime: 295 ms - Result correct: true
Query 2a >>              Runtime: 650 ms - Result correct: true
Query 2b >>              Runtime: 618 ms - Result correct: true
Query 2c >>              Runtime: 698 ms - Result correct: true
Query 2d >>              Runtime: 794 ms - Result correct: true
Query 3a >>              Runtime: 338 ms - Result correct: true
Query 3b >>              Runtime: 292 ms - Result correct: true
Query 3c >>              Runtime: 393 ms - Result correct: true
Query 4a >>              Runtime: 350 ms - Result correct: true
Query 4b >>              Runtime: 261 ms - Result correct: true
Query 4c >>              Runtime: 424 ms - Result correct: true
Query 5a >>              Runtime: 80 ms - Result correct: true
Query 5b >>              Runtime: 55 ms - Result correct: true
Query 5c >>              Runtime: 193 ms - Result correct: true
Query 6a >>              Runtime: 3081 ms - Result correct: true
Query 6b >>              Runtime: 2324 ms - Result correct: true
Query 6c >>              Runtime: 2110 ms - Result correct: true
Query 6d >>              Runtime: 2158 ms - Result correct: true
Query 6e >>              Runtime: 2152 ms - Result correct: true
Query 6f >>              Runtime: 3386 ms - Result correct: true
Query 7a >>              Runtime: 5335 ms - Result correct: true
Query 7b >>              Runtime: 2178 ms - Result correct: true
Query 7c >>              Runtime: 2838 ms - Result correct: true
Query 8a >>              Runtime: 257 ms - Result correct: true
Query 8b >>              Runtime: 71 ms - Result correct: true
Query 8c >>              Runtime: 5802 ms - Result correct: true
Query 8d >>              Runtime: 4600 ms - Result correct: true
Query 9a >>              Runtime: 470 ms - Result correct: true
Query 9b >>              Runtime: 387 ms - Result correct: true
Query 9c >>              Runtime: 1018 ms - Result correct: true
Query 9d >>              Runtime: 1552 ms - Result correct: true
Query 10a >>             Runtime: 454 ms - Result correct: true
Query 10b >>             Runtime: 850 ms - Result correct: true
Query 10c >>             Runtime: 1462 ms - Result correct: true
Query 11a >>             Runtime: 427 ms - Result correct: true
Query 11b >>             Runtime: 298 ms - Result correct: true
Query 11c >>             Runtime: 540 ms - Result correct: true
Query 11d >>             Runtime: 555 ms - Result correct: true
Query 12a >>             Runtime: 227 ms - Result correct: true
Query 12b >>             Runtime: 1576 ms - Result correct: true
Query 12c >>             Runtime: 320 ms - Result correct: true
Query 13a >>             Runtime: 2236 ms - Result correct: true
Query 13b >>             Runtime: 1122 ms - Result correct: true
Query 13c >>             Runtime: 1091 ms - Result correct: true
Query 13d >>             Runtime: 3334 ms - Result correct: true
Query 14a >>             Runtime: 425 ms - Result correct: true
Query 14b >>             Runtime: 298 ms - Result correct: true
Query 14c >>             Runtime: 471 ms - Result correct: true
Query 15a >>             Runtime: 352 ms - Result correct: true
Query 15b >>             Runtime: 309 ms - Result correct: true
Query 15c >>             Runtime: 537 ms - Result correct: true
Query 15d >>             Runtime: 555 ms - Result correct: true
Query 16a >>             Runtime: 3239 ms - Result correct: true
Query 16b >>             Runtime: 5354 ms - Result correct: true
Query 16c >>             Runtime: 2892 ms - Result correct: true
Query 16d >>             Runtime: 2890 ms - Result correct: true
Query 17a >>             Runtime: 2847 ms - Result correct: true
Query 17b >>             Runtime: 2345 ms - Result correct: true
Query 17c >>             Runtime: 2234 ms - Result correct: true
Query 17d >>             Runtime: 2243 ms - Result correct: true
Query 17e >>             Runtime: 3617 ms - Result correct: true
Query 17f >>             Runtime: 3357 ms - Result correct: true
Query 18a >>             Runtime: 1760 ms - Result correct: true
Query 18b >>             Runtime: 168 ms - Result correct: true
Query 18c >>             Runtime: 603 ms - Result correct: true
Query 19a >>             Runtime: 339 ms - Result correct: true
Query 19b >>             Runtime: 218 ms - Result correct: true
Query 19c >>             Runtime: 512 ms - Result correct: true
Query 19d >>             Runtime: 2821 ms - Result correct: true
Query 20a >>             Runtime: 2797 ms - Result correct: true
Query 20b >>             Runtime: 2458 ms - Result correct: true
Query 20c >>             Runtime: 3151 ms - Result correct: true
Query 21a >>             Runtime: 423 ms - Result correct: true
Query 21b >>             Runtime: 437 ms - Result correct: true
Query 21c >>             Runtime: 560 ms - Result correct: true
Query 22a >>             Runtime: 469 ms - Result correct: true
Query 22b >>             Runtime: 459 ms - Result correct: true
Query 22c >>             Runtime: 545 ms - Result correct: true
Query 22d >>             Runtime: 680 ms - Result correct: true
Query 23a >>             Runtime: 493 ms - Result correct: true
Query 23b >>             Runtime: 500 ms - Result correct: true
Query 23c >>             Runtime: 527 ms - Result correct: true
Query 24a >>             Runtime: 736 ms - Result correct: true
Query 24b >>             Runtime: 692 ms - Result correct: true
Query 25a >>             Runtime: 734 ms - Result correct: true
Query 25b >>             Runtime: 499 ms - Result correct: true
Query 25c >>             Runtime: 806 ms - Result correct: true
Query 26a >>             Runtime: 3555 ms - Result correct: true
Query 26b >>             Runtime: 2946 ms - Result correct: true
Query 26c >>             Runtime: 3622 ms - Result correct: true
Query 27a >>             Runtime: 393 ms - Result correct: true
Query 27b >>             Runtime: 321 ms - Result correct: true
Query 27c >>             Runtime: 479 ms - Result correct: true
Query 28a >>             Runtime: 556 ms - Result correct: true
Query 28b >>             Runtime: 379 ms - Result correct: true
Query 28c >>             Runtime: 495 ms - Result correct: true
Query 29a >>             Runtime: 635 ms - Result correct: true
Query 29b >>             Runtime: 648 ms - Result correct: true
Query 29c >>             Runtime: 1034 ms - Result correct: true
Query 30a >>             Runtime: 653 ms - Result correct: true
Query 30b >>             Runtime: 526 ms - Result correct: true
Query 30c >>             Runtime: 744 ms - Result correct: true
Query 31a >>             Runtime: 861 ms - Result correct: true
Query 31b >>             Runtime: 517 ms - Result correct: true
Query 31c >>             Runtime: 1033 ms - Result correct: true
Query 32a >>             Runtime: 555 ms - Result correct: true
Query 32b >>             Runtime: 622 ms - Result correct: true
Query 33a >>             Runtime: 650 ms - Result correct: true
Query 33b >>             Runtime: 640 ms - Result correct: true
Query 33c >>             Runtime: 1101 ms - Result correct: true
Total runtime: 140716 ms
```

## RHMap run plans.json
```bash
Query 1a >>              Runtime: 385 ms - Result correct: true
Query 1b >>              Runtime: 255 ms - Result correct: true
Query 1c >>              Runtime: 130 ms - Result correct: true
Query 1d >>              Runtime: 297 ms - Result correct: true
Query 2a >>              Runtime: 655 ms - Result correct: true
Query 2b >>              Runtime: 548 ms - Result correct: true
Query 2c >>              Runtime: 542 ms - Result correct: true
Query 2d >>              Runtime: 721 ms - Result correct: true
Query 3a >>              Runtime: 320 ms - Result correct: true
Query 3b >>              Runtime: 276 ms - Result correct: true
Query 3c >>              Runtime: 393 ms - Result correct: true
Query 4a >>              Runtime: 360 ms - Result correct: true
Query 4b >>              Runtime: 272 ms - Result correct: true
Query 4c >>              Runtime: 434 ms - Result correct: true
Query 5a >>              Runtime: 78 ms - Result correct: true
Query 5b >>              Runtime: 54 ms - Result correct: true
Query 5c >>              Runtime: 174 ms - Result correct: true
Query 6a >>              Runtime: 2443 ms - Result correct: true
Query 6b >>              Runtime: 2046 ms - Result correct: true
Query 6c >>              Runtime: 2411 ms - Result correct: true
Query 6d >>              Runtime: 2207 ms - Result correct: true
Query 6e >>              Runtime: 2558 ms - Result correct: true
Query 6f >>              Runtime: 3248 ms - Result correct: true
Query 7a >>              Runtime: 4091 ms - Result correct: true
Query 7b >>              Runtime: 1858 ms - Result correct: true
Query 7c >>              Runtime: 2437 ms - Result correct: true
Query 8a >>              Runtime: 249 ms - Result correct: true
Query 8b >>              Runtime: 69 ms - Result correct: true
Query 8c >>              Runtime: 5446 ms - Result correct: true
Query 8d >>              Runtime: 4386 ms - Result correct: true
Query 9a >>              Runtime: 433 ms - Result correct: true
Query 9b >>              Runtime: 373 ms - Result correct: true
Query 9c >>              Runtime: 1335 ms - Result correct: true
Query 9d >>              Runtime: 2056 ms - Result correct: true
Query 10a >>             Runtime: 443 ms - Result correct: true
Query 10b >>             Runtime: 1174 ms - Result correct: true
Query 10c >>             Runtime: 1748 ms - Result correct: true
Query 11a >>             Runtime: 414 ms - Result correct: true
Query 11b >>             Runtime: 293 ms - Result correct: true
Query 11c >>             Runtime: 509 ms - Result correct: true
Query 11d >>             Runtime: 545 ms - Result correct: true
Query 12a >>             Runtime: 217 ms - Result correct: true
Query 12b >>             Runtime: 1549 ms - Result correct: true
Query 12c >>             Runtime: 302 ms - Result correct: true
Query 13a >>             Runtime: 2273 ms - Result correct: true
Query 13b >>             Runtime: 1070 ms - Result correct: true
Query 13c >>             Runtime: 1058 ms - Result correct: true
Query 13d >>             Runtime: 3357 ms - Result correct: true
Query 14a >>             Runtime: 417 ms - Result correct: true
Query 14b >>             Runtime: 305 ms - Result correct: true
Query 14c >>             Runtime: 467 ms - Result correct: true
Query 15a >>             Runtime: 340 ms - Result correct: true
Query 15b >>             Runtime: 287 ms - Result correct: true
Query 15c >>             Runtime: 514 ms - Result correct: true
Query 15d >>             Runtime: 531 ms - Result correct: true
Query 16a >>             Runtime: 3282 ms - Result correct: true
Query 16b >>             Runtime: 4812 ms - Result correct: true
Query 16c >>             Runtime: 2520 ms - Result correct: true
Query 16d >>             Runtime: 2452 ms - Result correct: true
Query 17a >>             Runtime: 2737 ms - Result correct: true
Query 17b >>             Runtime: 2143 ms - Result correct: true
Query 17c >>             Runtime: 2061 ms - Result correct: true
Query 17d >>             Runtime: 2111 ms - Result correct: true
Query 17e >>             Runtime: 3263 ms - Result correct: true
Query 17f >>             Runtime: 2828 ms - Result correct: true
Query 18a >>             Runtime: 1566 ms - Result correct: true
Query 18b >>             Runtime: 167 ms - Result correct: true
Query 18c >>             Runtime: 510 ms - Result correct: true
Query 19a >>             Runtime: 312 ms - Result correct: true
Query 19b >>             Runtime: 209 ms - Result correct: true
Query 19c >>             Runtime: 459 ms - Result correct: true
Query 19d >>             Runtime: 3168 ms - Result correct: true
Query 20a >>             Runtime: 2615 ms - Result correct: true
Query 20b >>             Runtime: 2372 ms - Result correct: true
Query 20c >>             Runtime: 2798 ms - Result correct: true
Query 21a >>             Runtime: 415 ms - Result correct: true
Query 21b >>             Runtime: 427 ms - Result correct: true
Query 21c >>             Runtime: 550 ms - Result correct: true
Query 22a >>             Runtime: 485 ms - Result correct: true
Query 22b >>             Runtime: 478 ms - Result correct: true
Query 22c >>             Runtime: 539 ms - Result correct: true
Query 22d >>             Runtime: 641 ms - Result correct: true
Query 23a >>             Runtime: 455 ms - Result correct: true
Query 23b >>             Runtime: 469 ms - Result correct: true
Query 23c >>             Runtime: 479 ms - Result correct: true
Query 24a >>             Runtime: 675 ms - Result correct: true
Query 24b >>             Runtime: 648 ms - Result correct: true
Query 25a >>             Runtime: 634 ms - Result correct: true
Query 25b >>             Runtime: 479 ms - Result correct: true
Query 25c >>             Runtime: 725 ms - Result correct: true
Query 26a >>             Runtime: 3239 ms - Result correct: true
Query 26b >>             Runtime: 2696 ms - Result correct: true
Query 26c >>             Runtime: 3682 ms - Result correct: true
Query 27a >>             Runtime: 385 ms - Result correct: true
Query 27b >>             Runtime: 335 ms - Result correct: true
Query 27c >>             Runtime: 479 ms - Result correct: true
Query 28a >>             Runtime: 542 ms - Result correct: true
Query 28b >>             Runtime: 356 ms - Result correct: true
Query 28c >>             Runtime: 475 ms - Result correct: true
Query 29a >>             Runtime: 613 ms - Result correct: true
Query 29b >>             Runtime: 604 ms - Result correct: true
Query 29c >>             Runtime: 994 ms - Result correct: true
Query 30a >>             Runtime: 589 ms - Result correct: true
Query 30b >>             Runtime: 502 ms - Result correct: true
Query 30c >>             Runtime: 718 ms - Result correct: true
Query 31a >>             Runtime: 792 ms - Result correct: true
Query 31b >>             Runtime: 491 ms - Result correct: true
Query 31c >>             Runtime: 987 ms - Result correct: true
Query 32a >>             Runtime: 562 ms - Result correct: true
Query 32b >>             Runtime: 585 ms - Result correct: true
Query 33a >>             Runtime: 634 ms - Result correct: true
Query 33b >>             Runtime: 577 ms - Result correct: true
Query 33c >>             Runtime: 885 ms - Result correct: true
Total runtime: 133559 ms
```

## HopscotchMap run plans.json
```bash
Query 1a >>              Runtime: 381 ms - Result correct: true
Query 1b >>              Runtime: 265 ms - Result correct: true
Query 1c >>              Runtime: 135 ms - Result correct: true
Query 1d >>              Runtime: 293 ms - Result correct: true
Query 2a >>              Runtime: 666 ms - Result correct: true
Query 2b >>              Runtime: 605 ms - Result correct: true
Query 2c >>              Runtime: 597 ms - Result correct: true
Query 2d >>              Runtime: 795 ms - Result correct: true
Query 3a >>              Runtime: 313 ms - Result correct: true
Query 3b >>              Runtime: 265 ms - Result correct: true
Query 3c >>              Runtime: 382 ms - Result correct: true
Query 4a >>              Runtime: 362 ms - Result correct: true
Query 4b >>              Runtime: 261 ms - Result correct: true
Query 4c >>              Runtime: 439 ms - Result correct: true
Query 5a >>              Runtime: 78 ms - Result correct: true
Query 5b >>              Runtime: 55 ms - Result correct: true
Query 5c >>              Runtime: 170 ms - Result correct: true
Query 6a >>              Runtime: 2559 ms - Result correct: true
Query 6b >>              Runtime: 2074 ms - Result correct: true
Query 6c >>              Runtime: 2558 ms - Result correct: true
Query 6d >>              Runtime: 2194 ms - Result correct: true
Query 6e >>              Runtime: 2169 ms - Result correct: true
Query 6f >>              Runtime: 2941 ms - Result correct: true
Query 7a >>              Runtime: 3876 ms - Result correct: true
Query 7b >>              Runtime: 1902 ms - Result correct: true
Query 7c >>              Runtime: 2633 ms - Result correct: true
Query 8a >>              Runtime: 258 ms - Result correct: true
Query 8b >>              Runtime: 90 ms - Result correct: true
Query 8c >>              Runtime: 4770 ms - Result correct: true
Query 8d >>              Runtime: 3639 ms - Result correct: true
Query 9a >>              Runtime: 401 ms - Result correct: true
Query 9b >>              Runtime: 352 ms - Result correct: true
Query 9c >>              Runtime: 979 ms - Result correct: true
Query 9d >>              Runtime: 1599 ms - Result correct: true
Query 10a >>             Runtime: 449 ms - Result correct: true
Query 10b >>             Runtime: 810 ms - Result correct: true
Query 10c >>             Runtime: 1353 ms - Result correct: true
Query 11a >>             Runtime: 424 ms - Result correct: true
Query 11b >>             Runtime: 313 ms - Result correct: true
Query 11c >>             Runtime: 535 ms - Result correct: true
Query 11d >>             Runtime: 555 ms - Result correct: true
Query 12a >>             Runtime: 224 ms - Result correct: true
Query 12b >>             Runtime: 1559 ms - Result correct: true
Query 12c >>             Runtime: 370 ms - Result correct: true
Query 13a >>             Runtime: 2220 ms - Result correct: true
Query 13b >>             Runtime: 1054 ms - Result correct: true
Query 13c >>             Runtime: 1055 ms - Result correct: true
Query 13d >>             Runtime: 3139 ms - Result correct: true
Query 14a >>             Runtime: 412 ms - Result correct: true
Query 14b >>             Runtime: 304 ms - Result correct: true
Query 14c >>             Runtime: 463 ms - Result correct: true
Query 15a >>             Runtime: 325 ms - Result correct: true
Query 15b >>             Runtime: 307 ms - Result correct: true
Query 15c >>             Runtime: 506 ms - Result correct: true
Query 15d >>             Runtime: 522 ms - Result correct: true
Query 16a >>             Runtime: 2898 ms - Result correct: true
Query 16b >>             Runtime: 4838 ms - Result correct: true
Query 16c >>             Runtime: 2517 ms - Result correct: true
Query 16d >>             Runtime: 2460 ms - Result correct: true
Query 17a >>             Runtime: 2704 ms - Result correct: true
Query 17b >>             Runtime: 2199 ms - Result correct: true
Query 17c >>             Runtime: 2101 ms - Result correct: true
Query 17d >>             Runtime: 2110 ms - Result correct: true
Query 17e >>             Runtime: 3181 ms - Result correct: true
Query 17f >>             Runtime: 2938 ms - Result correct: true
Query 18a >>             Runtime: 1606 ms - Result correct: true
Query 18b >>             Runtime: 164 ms - Result correct: true
Query 18c >>             Runtime: 605 ms - Result correct: true
Query 19a >>             Runtime: 299 ms - Result correct: true
Query 19b >>             Runtime: 204 ms - Result correct: true
Query 19c >>             Runtime: 504 ms - Result correct: true
Query 19d >>             Runtime: 2824 ms - Result correct: true
Query 20a >>             Runtime: 2776 ms - Result correct: true
Query 20b >>             Runtime: 2521 ms - Result correct: true
Query 20c >>             Runtime: 2986 ms - Result correct: true
Query 21a >>             Runtime: 426 ms - Result correct: true
Query 21b >>             Runtime: 421 ms - Result correct: true
Query 21c >>             Runtime: 517 ms - Result correct: true
Query 22a >>             Runtime: 454 ms - Result correct: true
Query 22b >>             Runtime: 457 ms - Result correct: true
Query 22c >>             Runtime: 525 ms - Result correct: true
Query 22d >>             Runtime: 652 ms - Result correct: true
Query 23a >>             Runtime: 501 ms - Result correct: true
Query 23b >>             Runtime: 480 ms - Result correct: true
Query 23c >>             Runtime: 511 ms - Result correct: true
Query 24a >>             Runtime: 724 ms - Result correct: true
Query 24b >>             Runtime: 681 ms - Result correct: true
Query 25a >>             Runtime: 671 ms - Result correct: true
Query 25b >>             Runtime: 481 ms - Result correct: true
Query 25c >>             Runtime: 729 ms - Result correct: true
Query 26a >>             Runtime: 3509 ms - Result correct: true
Query 26b >>             Runtime: 2790 ms - Result correct: true
Query 26c >>             Runtime: 3469 ms - Result correct: true
Query 27a >>             Runtime: 536 ms - Result correct: true
Query 27b >>             Runtime: 489 ms - Result correct: true
Query 27c >>             Runtime: 614 ms - Result correct: true
Query 28a >>             Runtime: 705 ms - Result correct: true
Query 28b >>             Runtime: 522 ms - Result correct: true
Query 28c >>             Runtime: 631 ms - Result correct: true
Query 29a >>             Runtime: 846 ms - Result correct: true
Query 29b >>             Runtime: 802 ms - Result correct: true
Query 29c >>             Runtime: 1133 ms - Result correct: true
Query 30a >>             Runtime: 763 ms - Result correct: true
Query 30b >>             Runtime: 647 ms - Result correct: true
Query 30c >>             Runtime: 855 ms - Result correct: true
Query 31a >>             Runtime: 827 ms - Result correct: true
Query 31b >>             Runtime: 506 ms - Result correct: true
Query 31c >>             Runtime: 969 ms - Result correct: true
Query 32a >>             Runtime: 555 ms - Result correct: true
Query 32b >>             Runtime: 583 ms - Result correct: true
Query 33a >>             Runtime: 622 ms - Result correct: true
Query 33b >>             Runtime: 676 ms - Result correct: true
Query 33c >>             Runtime: 820 ms - Result correct: true
Total runtime: 132489 ms
```