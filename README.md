# Optimized Hash Join Pipeline

A high-performance hash join execution engine for the [Join Order Benchmark (JOB)](http://www.vldb.org/pvldb/vol9/p204-leis.pdf), achieving a **59x speedup** over the baseline implementation on the IMDB dataset.

Built on top of [TEAMOPT's columnar storage framework](https://github.com/sigmod25-teamOPT/optimized-join-pipeline) as part of a course project at the University of Athens.

![Total Runtime Comparison](total_runtime_comparison_chart.png)

## Results

| Version | Implementation | Total Runtime | Speedup |
|---------|---------------|--------------|---------|
| v1.0.0 | Baseline (`std::unordered_map`) | 130.0 s | 1.0x |
| v2.0.0 | Unchained Hash Table | 12.9 s | 10.1x |
| v3.0.0 | + Direct Page Access | 7.1 s | 18.3x |
| v3.0.0 | + Parallel Build | 6.8 s | 19.1x |
| v3.0.0 | + Parallel Probe (Work-Stealing) | 3.0 s | 43.3x |
| v3.0.0 | + Thread Pool + mmap Allocator | 2.2 s | **59.1x** |

> Benchmarked on Intel Xeon E5-2680 v3 (24 threads), all 113 JOB queries (1a–33c).

### SIGMOD 2025 Contest Comparison

All solutions compiled and benchmarked locally on the same machine (i7-13700K, 12 threads, 5 GB RAM). Best of 3 runs reported.

| Rank | Team | Affiliation | Runtime |
|------|------|-------------|---------|
| 1st | [SortMergeJoins](https://github.com/umbra-db/contest-sigmod2025) | TU Munich | 299 ms |
| 2nd | [Kirara](https://github.com/dawnex3/SIGMOD2025Contest-Kirara) | Beijing IT / Xidian | 1,319 ms |
| 3rd | [Embryo](https://github.com/embryo-labs/SIGMOD2025Contest-Embryo) | Tsinghua | 1,584 ms |
| 4th | [O.P.T.](https://github.com/sigmod25-teamOPT/optimized-join-pipeline) | U. Athens | 1,693 ms |
| — | **Ours** | U. Athens | **2,161 ms** |
| 5th | [JobSeeking](https://github.com/SIGMOD-25-Programming-Contest/JobSeeking) | UC Riverside | 3,465 ms |

> Our implementation places between 4th and 5th. SortMergeJoins (TU Munich / Umbra) uses a full query engine with sort-merge joins and columnar storage.

---

## Architecture

The engine executes pre-optimized query plans (`plans.json`) for the JOB benchmark. Each plan specifies join order, build/probe sides, and output columns.

```
Load  →  Build  →  Probe  →  Materialize
 CSV     hash table   lookup     output columns
         (parallel)   (work-     (parallel)
                       stealing)
```

1. **Load** — Parse CSV data into columnar storage; maintain direct page references for non-null INT32 columns
2. **Build** — Construct unchained hash table on the build side (parallel, partitioned)
3. **Probe** — Scan probe side and look up matches (parallel, work-stealing)
4. **Materialize** — Collect matched tuples into output columns (parallel)

---

## Optimizations

### Unchained Hash Table (v2.0.0 — 10x)

Custom open-addressing hash table based on Birler et al. [[1]](#references):

- **Flat storage** — All entries in a contiguous array for cache-friendly access
- **Bloom filter tags** — 16-bit tags (4 bits set) from a 2048-entry lookup table; mismatches skip slots without touching entry data
- **CRC32 hashing** — Hardware-accelerated via `_mm_crc32_u64`
- **Late materialization** — Entries store only `(key, row_idx)`, deferring tuple construction until a match is confirmed

### Direct Page Access (v3.0.0 — 1.8x)

For non-null INT32 columns, bypasses buffering entirely. Pre-computed `page_rowids` mappings provide O(1) row-to-page access; values are read directly from original column pages during probing.

### Parallel Build (v3.0.0 — 1.04x)

Three-phase construction (partition → count → insert) with a three-level slab allocator:

| Level | Component | Size | Role |
|-------|-----------|------|------|
| 1 | `GlobalAllocator` | 64 KB | `mmap` arena with `MADV_HUGEPAGE`, lock-free allocation |
| 2 | `BumpAllocator` | 4 KB | Per-thread bump allocation from Level 1 chunks |
| 3 | Per-Partition | Per-tuple | Entry storage within Level 2 chunks |

Each thread partitions into private buckets (no synchronization), then a prefix-sum pass and a copy pass finalize positions and compute Bloom filter tags.

### Parallel Probe with Work-Stealing (v3.0.0 — 2.3x)

A single `std::atomic<size_t>` counter implements lock-free work-stealing. Each thread claims pages via `fetch_add` and processes them; threads that finish early pick up remaining work automatically.

```cpp
thread_pool.run_work_stealing(num_pages, [&](size_t tid, size_t page_idx) {
    // Process page_idx — threads claim pages dynamically
});
```

### Thread Pool & mmap Allocator (v3.0.0 — 1.4x)

A global `ThreadPool` with generation-based dispatch keeps workers alive across all joins and queries, eliminating ~1 ms thread-creation overhead per join. The `GlobalAllocator` reuses a single `mmap` arena across queries via `reset()`.

---

## Getting Started

### Prerequisites

- C++17 compiler (GCC 9+ or Clang 10+)
- CMake 3.16+
- Linux (uses `mmap`, `<linux/version.h>`)

### Quick Start

```bash
make setup        # Download IMDB dataset + build binary cache (one-time)
make              # Build optimized executable
make run          # Run all 113 JOB queries
```

### All Targets

| Target | Description |
|--------|-------------|
| `make` | Build (Release) |
| `make run` | Build and run all queries |
| `make tests` | Build unit tests |
| `make run-tests` | Build and run unit tests |
| `make clean` | Remove build directory |
| `make download-imdb` | Download IMDB CSV files only |
| `make setup` | Download IMDB + build binary cache |

---

## Project Structure

```
include/
  unchained_ht.h      Unchained hash table with Bloom filter tags
  utils.h             ThreadPool, GlobalAllocator, column_t, buffer_t
  table.h             Table representation
  plan.h              Query plan parsing
  statement.h         SQL statement types
src/
  execute.cpp          Core join execution (build, probe, materialize)
  build_table.cpp      Table loading and column construction
  csv_parser.cpp       CSV file parser
  statement.cpp        Statement handling
tests/
  unit_tests.cpp       Join correctness tests (empty, nulls, duplicates, multi-column)
  unchained_tests.cpp  Hash table tests (lookup, miss, collisions)
  read_sql.cpp         SQL query runner with DuckDB verification
  build_database.cpp   IMDB database builder
job/                   JOB benchmark queries (1a–33c)
plans.json             Pre-optimized query execution plans
Makefile               Build and run targets
```

---

## Version History

| Tag | Description |
|-----|-------------|
| **v1.0.0** | Baseline hash join engine using `std::unordered_map`. Also explored Robin Hood, Hopscotch, and Cuckoo hash maps. |
| **v2.0.0** | Unchained hash table with Bloom filter tags and CRC32 hashing (10x). |
| **v3.0.0** | Parallel build/probe, work-stealing, direct page access, thread pool, mmap allocator (59x). |

---

## References

1. A. Birler, T. Schmidt, P. Fent, and T. Neumann. *"Simple, Efficient, and Robust Hash Tables for Join Processing."* DaMoN '24, Article 4, pp. 1–9. [doi:10.1145/3662010.3663442](https://doi.org/10.1145/3662010.3663442)
2. P. A. Boncz, S. Manegold, and M. L. Kersten. *"Database Architecture Optimized for the New Bottleneck: Memory Access."* VLDB '99, pp. 54–65. [paper](https://www.vldb.org/conf/1999/P5.pdf)
3. V. Leis, A. Gubichev, A. Mirber, P. Boncz, A. Kemper, and T. Neumann. *"How Good Are Query Optimizers, Really?"* PVLDB, Vol. 9, 2015. [paper](http://www.vldb.org/pvldb/vol9/p204-leis.pdf)

## Acknowledgements

This project was built on top of [TEAMOPT's Optimized Join Pipeline](https://github.com/sigmod25-teamOPT/optimized-join-pipeline), which provides the columnar storage framework, query plan infrastructure, and DuckDB-based result verification used throughout this work. TEAMOPT's codebase served as the baseline for the [SIGMOD 2025 Programming Contest](https://sigmod-contest-2025.github.io/) and was provided as part of a university course at the University of Athens.

## Authors

- **Stavros Gkousgkounis** — [StavrosGous](https://github.com/StavrosGous)
- **Ioannis Vogiatzis** — [JohnVogia](https://github.com/JohnVogia)
