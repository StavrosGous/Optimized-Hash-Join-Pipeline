# Team Members:
- Ioannis Vogiatzis (11152200021)
- Stavros Gkousgkounis (11152200033)

# Responsibilities

It is difficult to split responsibilities because we both have been involved with all parts of the code to some extent.

# Fast Plans Runtime Benchmarks

## Fast, Default Implementation
Runtime measurements captured from `./build/fast plans.json` which includes test queries `1a-33c` had a mean total runtime of `130.0` seconds.
In detail one indicative run had the following durations:
```bash
$ cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -Wno-dev
$ cmake --build build -- -j $(nproc) fast
$ ./build/fast plans.json
Query 1a >>              Runtime: 390 ms - Result correct: true
Query 1b >>              Runtime: 263 ms - Result correct: true
Query 1c >>              Runtime: 131 ms - Result correct: true
Query 1d >>              Runtime: 299 ms - Result correct: true
Query 2a >>              Runtime: 659 ms - Result correct: true
Query 2b >>              Runtime: 674 ms - Result correct: true
Query 2c >>              Runtime: 631 ms - Result correct: true
Query 2d >>              Runtime: 743 ms - Result correct: true
Query 3a >>              Runtime: 311 ms - Result correct: true
Query 3b >>              Runtime: 264 ms - Result correct: true
Query 3c >>              Runtime: 386 ms - Result correct: true
Query 4a >>              Runtime: 351 ms - Result correct: true
Query 4b >>              Runtime: 257 ms - Result correct: true
Query 4c >>              Runtime: 433 ms - Result correct: true
Query 5a >>              Runtime: 77 ms - Result correct: true
Query 5b >>              Runtime: 52 ms - Result correct: true
Query 5c >>              Runtime: 176 ms - Result correct: true
Query 6a >>              Runtime: 2462 ms - Result correct: true
Query 6b >>              Runtime: 2022 ms - Result correct: true
Query 6c >>              Runtime: 1856 ms - Result correct: true
Query 6d >>              Runtime: 1984 ms - Result correct: true
Query 6e >>              Runtime: 1928 ms - Result correct: true
Query 6f >>              Runtime: 3114 ms - Result correct: true
Query 7a >>              Runtime: 4469 ms - Result correct: true
Query 7b >>              Runtime: 1811 ms - Result correct: true
Query 7c >>              Runtime: 2645 ms - Result correct: true
Query 8a >>              Runtime: 250 ms - Result correct: true
Query 8b >>              Runtime: 71 ms - Result correct: true
Query 8c >>              Runtime: 5079 ms - Result correct: true
Query 8d >>              Runtime: 4056 ms - Result correct: true
Query 9a >>              Runtime: 464 ms - Result correct: true
Query 9b >>              Runtime: 382 ms - Result correct: true
Query 9c >>              Runtime: 963 ms - Result correct: true
Query 9d >>              Runtime: 1517 ms - Result correct: true
Query 10a >>             Runtime: 447 ms - Result correct: true
Query 10b >>             Runtime: 796 ms - Result correct: true
Query 10c >>             Runtime: 1337 ms - Result correct: true
Query 11a >>             Runtime: 414 ms - Result correct: true
Query 11b >>             Runtime: 297 ms - Result correct: true
Query 11c >>             Runtime: 517 ms - Result correct: true
Query 11d >>             Runtime: 543 ms - Result correct: true
Query 12a >>             Runtime: 218 ms - Result correct: true
Query 12b >>             Runtime: 1502 ms - Result correct: true
Query 12c >>             Runtime: 338 ms - Result correct: true
Query 13a >>             Runtime: 2132 ms - Result correct: true
Query 13b >>             Runtime: 1067 ms - Result correct: true
Query 13c >>             Runtime: 1031 ms - Result correct: true
Query 13d >>             Runtime: 3067 ms - Result correct: true
Query 14a >>             Runtime: 396 ms - Result correct: true
Query 14b >>             Runtime: 274 ms - Result correct: true
Query 14c >>             Runtime: 465 ms - Result correct: true
Query 15a >>             Runtime: 334 ms - Result correct: true
Query 15b >>             Runtime: 314 ms - Result correct: true
Query 15c >>             Runtime: 509 ms - Result correct: true
Query 15d >>             Runtime: 513 ms - Result correct: true
Query 16a >>             Runtime: 2954 ms - Result correct: true
Query 16b >>             Runtime: 4799 ms - Result correct: true
Query 16c >>             Runtime: 2641 ms - Result correct: true
Query 16d >>             Runtime: 2590 ms - Result correct: true
Query 17a >>             Runtime: 2648 ms - Result correct: true
Query 17b >>             Runtime: 2171 ms - Result correct: true
Query 17c >>             Runtime: 2074 ms - Result correct: true
Query 17d >>             Runtime: 2101 ms - Result correct: true
Query 17e >>             Runtime: 3329 ms - Result correct: true
Query 17f >>             Runtime: 3132 ms - Result correct: true
Query 18a >>             Runtime: 1566 ms - Result correct: true
Query 18b >>             Runtime: 171 ms - Result correct: true
Query 18c >>             Runtime: 566 ms - Result correct: true
Query 19a >>             Runtime: 326 ms - Result correct: true
Query 19b >>             Runtime: 211 ms - Result correct: true
Query 19c >>             Runtime: 492 ms - Result correct: true
Query 19d >>             Runtime: 2698 ms - Result correct: true
Query 20a >>             Runtime: 2696 ms - Result correct: true
Query 20b >>             Runtime: 2357 ms - Result correct: true
Query 20c >>             Runtime: 2928 ms - Result correct: true
Query 21a >>             Runtime: 409 ms - Result correct: true
Query 21b >>             Runtime: 424 ms - Result correct: true
Query 21c >>             Runtime: 548 ms - Result correct: true
Query 22a >>             Runtime: 472 ms - Result correct: true
Query 22b >>             Runtime: 458 ms - Result correct: true
Query 22c >>             Runtime: 542 ms - Result correct: true
Query 22d >>             Runtime: 659 ms - Result correct: true
Query 23a >>             Runtime: 493 ms - Result correct: true
Query 23b >>             Runtime: 485 ms - Result correct: true
Query 23c >>             Runtime: 531 ms - Result correct: true
Query 24a >>             Runtime: 761 ms - Result correct: true
Query 24b >>             Runtime: 675 ms - Result correct: true
Query 25a >>             Runtime: 734 ms - Result correct: true
Query 25b >>             Runtime: 484 ms - Result correct: true
Query 25c >>             Runtime: 793 ms - Result correct: true
Query 26a >>             Runtime: 3352 ms - Result correct: true
Query 26b >>             Runtime: 2667 ms - Result correct: true
Query 26c >>             Runtime: 3327 ms - Result correct: true
Query 27a >>             Runtime: 379 ms - Result correct: true
Query 27b >>             Runtime: 297 ms - Result correct: true
Query 27c >>             Runtime: 450 ms - Result correct: true
Query 28a >>             Runtime: 509 ms - Result correct: true
Query 28b >>             Runtime: 359 ms - Result correct: true
Query 28c >>             Runtime: 473 ms - Result correct: true
Query 29a >>             Runtime: 589 ms - Result correct: true
Query 29b >>             Runtime: 574 ms - Result correct: true
Query 29c >>             Runtime: 947 ms - Result correct: true
Query 30a >>             Runtime: 618 ms - Result correct: true
Query 30b >>             Runtime: 499 ms - Result correct: true
Query 30c >>             Runtime: 730 ms - Result correct: true
Query 31a >>             Runtime: 829 ms - Result correct: true
Query 31b >>             Runtime: 488 ms - Result correct: true
Query 31c >>             Runtime: 1015 ms - Result correct: true
Query 32a >>             Runtime: 545 ms - Result correct: true
Query 32b >>             Runtime: 598 ms - Result correct: true
Query 33a >>             Runtime: 617 ms - Result correct: true
Query 33b >>             Runtime: 619 ms - Result correct: true
Query 33c >>             Runtime: 1008 ms - Result correct: true
Total runtime: 130090 ms
```

## Fast with Unchained Hashtable (Stage 3)
Runtime measurements captured from `./run.sh fast_plans` which includes test queries `1a-33c` had a mean total runtime of `12.9` seconds.
In detail one indicative run had the following durations:
```bash
$ ./build.sh fast_plans
$ ./run.sh fast_plans
Query 1a >>              Runtime: 40 ms - Result correct: true
Query 1b >>              Runtime: 29 ms - Result correct: true
Query 1c >>              Runtime: 13 ms - Result correct: true
Query 1d >>              Runtime: 36 ms - Result correct: true
Query 2a >>              Runtime: 62 ms - Result correct: true
Query 2b >>              Runtime: 59 ms - Result correct: true
Query 2c >>              Runtime: 53 ms - Result correct: true
Query 2d >>              Runtime: 78 ms - Result correct: true
Query 3a >>              Runtime: 35 ms - Result correct: true
Query 3b >>              Runtime: 30 ms - Result correct: true
Query 3c >>              Runtime: 43 ms - Result correct: true
Query 4a >>              Runtime: 43 ms - Result correct: true
Query 4b >>              Runtime: 32 ms - Result correct: true
Query 4c >>              Runtime: 53 ms - Result correct: true
Query 5a >>              Runtime: 4 ms - Result correct: true
Query 5b >>              Runtime: 3 ms - Result correct: true
Query 5c >>              Runtime: 17 ms - Result correct: true
Query 6a >>              Runtime: 237 ms - Result correct: true
Query 6b >>              Runtime: 243 ms - Result correct: true
Query 6c >>              Runtime: 237 ms - Result correct: true
Query 6d >>              Runtime: 250 ms - Result correct: true
Query 6e >>              Runtime: 247 ms - Result correct: true
Query 6f >>              Runtime: 421 ms - Result correct: true
Query 7a >>              Runtime: 472 ms - Result correct: true
Query 7b >>              Runtime: 223 ms - Result correct: true
Query 7c >>              Runtime: 247 ms - Result correct: true
Query 8a >>              Runtime: 14 ms - Result correct: true
Query 8b >>              Runtime: 3 ms - Result correct: true
Query 8c >>              Runtime: 701 ms - Result correct: true
Query 8d >>              Runtime: 402 ms - Result correct: true
Query 9a >>              Runtime: 23 ms - Result correct: true
Query 9b >>              Runtime: 21 ms - Result correct: true
Query 9c >>              Runtime: 129 ms - Result correct: true
Query 9d >>              Runtime: 229 ms - Result correct: true
Query 10a >>             Runtime: 25 ms - Result correct: true
Query 10b >>             Runtime: 24 ms - Result correct: true
Query 10c >>             Runtime: 203 ms - Result correct: true
Query 11a >>             Runtime: 29 ms - Result correct: true
Query 11b >>             Runtime: 19 ms - Result correct: true
Query 11c >>             Runtime: 31 ms - Result correct: true
Query 11d >>             Runtime: 36 ms - Result correct: true
Query 12a >>             Runtime: 14 ms - Result correct: true
Query 12b >>             Runtime: 83 ms - Result correct: true
Query 12c >>             Runtime: 24 ms - Result correct: true
Query 13a >>             Runtime: 148 ms - Result correct: true
Query 13b >>             Runtime: 75 ms - Result correct: true
Query 13c >>             Runtime: 75 ms - Result correct: true
Query 13d >>             Runtime: 361 ms - Result correct: true
Query 14a >>             Runtime: 33 ms - Result correct: true
Query 14b >>             Runtime: 20 ms - Result correct: true
Query 14c >>             Runtime: 36 ms - Result correct: true
Query 15a >>             Runtime: 19 ms - Result correct: true
Query 15b >>             Runtime: 16 ms - Result correct: true
Query 15c >>             Runtime: 32 ms - Result correct: true
Query 15d >>             Runtime: 34 ms - Result correct: true
Query 16a >>             Runtime: 261 ms - Result correct: true
Query 16b >>             Runtime: 605 ms - Result correct: true
Query 16c >>             Runtime: 183 ms - Result correct: true
Query 16d >>             Runtime: 251 ms - Result correct: true
Query 17a >>             Runtime: 277 ms - Result correct: true
Query 17b >>             Runtime: 193 ms - Result correct: true
Query 17c >>             Runtime: 178 ms - Result correct: true
Query 17d >>             Runtime: 185 ms - Result correct: true
Query 17e >>             Runtime: 427 ms - Result correct: true
Query 17f >>             Runtime: 320 ms - Result correct: true
Query 18a >>             Runtime: 92 ms - Result correct: true
Query 18b >>             Runtime: 9 ms - Result correct: true
Query 18c >>             Runtime: 42 ms - Result correct: true
Query 19a >>             Runtime: 19 ms - Result correct: true
Query 19b >>             Runtime: 13 ms - Result correct: true
Query 19c >>             Runtime: 30 ms - Result correct: true
Query 19d >>             Runtime: 297 ms - Result correct: true
Query 20a >>             Runtime: 267 ms - Result correct: true
Query 20b >>             Runtime: 266 ms - Result correct: true
Query 20c >>             Runtime: 356 ms - Result correct: true
Query 21a >>             Runtime: 30 ms - Result correct: true
Query 21b >>             Runtime: 29 ms - Result correct: true
Query 21c >>             Runtime: 35 ms - Result correct: true
Query 22a >>             Runtime: 36 ms - Result correct: true
Query 22b >>             Runtime: 36 ms - Result correct: true
Query 22c >>             Runtime: 41 ms - Result correct: true
Query 22d >>             Runtime: 53 ms - Result correct: true
Query 23a >>             Runtime: 31 ms - Result correct: true
Query 23b >>             Runtime: 30 ms - Result correct: true
Query 23c >>             Runtime: 33 ms - Result correct: true
Query 24a >>             Runtime: 43 ms - Result correct: true
Query 24b >>             Runtime: 40 ms - Result correct: true
Query 25a >>             Runtime: 48 ms - Result correct: true
Query 25b >>             Runtime: 34 ms - Result correct: true
Query 25c >>             Runtime: 56 ms - Result correct: true
Query 26a >>             Runtime: 381 ms - Result correct: true
Query 26b >>             Runtime: 351 ms - Result correct: true
Query 26c >>             Runtime: 425 ms - Result correct: true
Query 27a >>             Runtime: 26 ms - Result correct: true
Query 27b >>             Runtime: 21 ms - Result correct: true
Query 27c >>             Runtime: 31 ms - Result correct: true
Query 28a >>             Runtime: 41 ms - Result correct: true
Query 28b >>             Runtime: 24 ms - Result correct: true
Query 28c >>             Runtime: 36 ms - Result correct: true
Query 29a >>             Runtime: 41 ms - Result correct: true
Query 29b >>             Runtime: 41 ms - Result correct: true
Query 29c >>             Runtime: 71 ms - Result correct: true
Query 30a >>             Runtime: 42 ms - Result correct: true
Query 30b >>             Runtime: 35 ms - Result correct: true
Query 30c >>             Runtime: 55 ms - Result correct: true
Query 31a >>             Runtime: 55 ms - Result correct: true
Query 31b >>             Runtime: 35 ms - Result correct: true
Query 31c >>             Runtime: 67 ms - Result correct: true
Query 32a >>             Runtime: 25 ms - Result correct: true
Query 32b >>             Runtime: 35 ms - Result correct: true
Query 33a >>             Runtime: 47 ms - Result correct: true
Query 33b >>             Runtime: 42 ms - Result correct: true
Query 33c >>             Runtime: 63 ms - Result correct: true
Total runtime: 12902 ms
```

## Fast with Optimized Indexing
<<<<<<< Updated upstream
Runtime measurements captured from `./run.sh fast_plans` which includes test queries `1a-33c` had a mean total runtime of `6.5` seconds.
=======
Runtime measurements captured from `./run.sh fast_plans` which includes test queries `1a-33c` had a mean total runtime of `7.1` seconds.
>>>>>>> Stashed changes
```bash
$ ./build.sh fast_plans
$ ./run.sh fast_plans
Query 1a >>              Runtime: 19 ms - Result correct: true
Query 1b >>              Runtime: 9 ms - Result correct: true
Query 1c >>              Runtime: 3 ms - Result correct: true
Query 1d >>              Runtime: 12 ms - Result correct: true
Query 2a >>              Runtime: 24 ms - Result correct: true
Query 2b >>              Runtime: 24 ms - Result correct: true
Query 2c >>              Runtime: 17 ms - Result correct: true
Query 2d >>              Runtime: 37 ms - Result correct: true
Query 3a >>              Runtime: 8 ms - Result correct: true
Query 3b >>              Runtime: 6 ms - Result correct: true
Query 3c >>              Runtime: 13 ms - Result correct: true
Query 4a >>              Runtime: 14 ms - Result correct: true
Query 4b >>              Runtime: 5 ms - Result correct: true
Query 4c >>              Runtime: 23 ms - Result correct: true
Query 5a >>              Runtime: 1 ms - Result correct: true
Query 5b >>              Runtime: 0 ms - Result correct: true
Query 5c >>              Runtime: 6 ms - Result correct: true
Query 6a >>              Runtime: 37 ms - Result correct: true
Query 6b >>              Runtime: 39 ms - Result correct: true
Query 6c >>              Runtime: 37 ms - Result correct: true
Query 6d >>              Runtime: 44 ms - Result correct: true
Query 6e >>              Runtime: 42 ms - Result correct: true
Query 6f >>              Runtime: 231 ms - Result correct: true
Query 7a >>              Runtime: 576 ms - Result correct: true
Query 7b >>              Runtime: 59 ms - Result correct: true
Query 7c >>              Runtime: 81 ms - Result correct: true
Query 8a >>              Runtime: 9 ms - Result correct: true
Query 8b >>              Runtime: 2 ms - Result correct: true
Query 8c >>              Runtime: 648 ms - Result correct: true
Query 8d >>              Runtime: 302 ms - Result correct: true
Query 9a >>              Runtime: 15 ms - Result correct: true
Query 9b >>              Runtime: 13 ms - Result correct: true
Query 9c >>              Runtime: 25 ms - Result correct: true
Query 9d >>              Runtime: 130 ms - Result correct: true
Query 10a >>             Runtime: 14 ms - Result correct: true
Query 10b >>             Runtime: 13 ms - Result correct: true
Query 10c >>             Runtime: 157 ms - Result correct: true
Query 11a >>             Runtime: 17 ms - Result correct: true
Query 11b >>             Runtime: 6 ms - Result correct: true
Query 11c >>             Runtime: 16 ms - Result correct: true
Query 11d >>             Runtime: 19 ms - Result correct: true
Query 12a >>             Runtime: 6 ms - Result correct: true
Query 12b >>             Runtime: 47 ms - Result correct: true
Query 12c >>             Runtime: 15 ms - Result correct: true
Query 13a >>             Runtime: 95 ms - Result correct: true
Query 13b >>             Runtime: 35 ms - Result correct: true
Query 13c >>             Runtime: 34 ms - Result correct: true
Query 13d >>             Runtime: 376 ms - Result correct: true
Query 14a >>             Runtime: 18 ms - Result correct: true
Query 14b >>             Runtime: 10 ms - Result correct: true
Query 14c >>             Runtime: 22 ms - Result correct: true
Query 15a >>             Runtime: 8 ms - Result correct: true
Query 15b >>             Runtime: 6 ms - Result correct: true
Query 15c >>             Runtime: 14 ms - Result correct: true
Query 15d >>             Runtime: 13 ms - Result correct: true
Query 16a >>             Runtime: 117 ms - Result correct: true
Query 16b >>             Runtime: 466 ms - Result correct: true
Query 16c >>             Runtime: 98 ms - Result correct: true
Query 16d >>             Runtime: 89 ms - Result correct: true
Query 17a >>             Runtime: 156 ms - Result correct: true
Query 17b >>             Runtime: 62 ms - Result correct: true
Query 17c >>             Runtime: 47 ms - Result correct: true
Query 17d >>             Runtime: 48 ms - Result correct: true
Query 17e >>             Runtime: 282 ms - Result correct: true
Query 17f >>             Runtime: 182 ms - Result correct: true
Query 18a >>             Runtime: 55 ms - Result correct: true
Query 18b >>             Runtime: 5 ms - Result correct: true
Query 18c >>             Runtime: 37 ms - Result correct: true
Query 19a >>             Runtime: 11 ms - Result correct: true
Query 19b >>             Runtime: 6 ms - Result correct: true
Query 19c >>             Runtime: 16 ms - Result correct: true
Query 19d >>             Runtime: 294 ms - Result correct: true
Query 20a >>             Runtime: 112 ms - Result correct: true
Query 20b >>             Runtime: 89 ms - Result correct: true
Query 20c >>             Runtime: 179 ms - Result correct: true
Query 21a >>             Runtime: 16 ms - Result correct: true
Query 21b >>             Runtime: 17 ms - Result correct: true
Query 21c >>             Runtime: 20 ms - Result correct: true
Query 22a >>             Runtime: 23 ms - Result correct: true
Query 22b >>             Runtime: 20 ms - Result correct: true
Query 22c >>             Runtime: 26 ms - Result correct: true
Query 22d >>             Runtime: 29 ms - Result correct: true
Query 23a >>             Runtime: 11 ms - Result correct: true
Query 23b >>             Runtime: 10 ms - Result correct: true
Query 23c >>             Runtime: 11 ms - Result correct: true
Query 24a >>             Runtime: 21 ms - Result correct: true
Query 24b >>             Runtime: 18 ms - Result correct: true
Query 25a >>             Runtime: 28 ms - Result correct: true
Query 25b >>             Runtime: 17 ms - Result correct: true
Query 25c >>             Runtime: 38 ms - Result correct: true
Query 26a >>             Runtime: 217 ms - Result correct: true
Query 26b >>             Runtime: 169 ms - Result correct: true
Query 26c >>             Runtime: 249 ms - Result correct: true
Query 27a >>             Runtime: 10 ms - Result correct: true
Query 27b >>             Runtime: 7 ms - Result correct: true
Query 27c >>             Runtime: 14 ms - Result correct: true
Query 28a >>             Runtime: 21 ms - Result correct: true
Query 28b >>             Runtime: 12 ms - Result correct: true
Query 28c >>             Runtime: 18 ms - Result correct: true
Query 29a >>             Runtime: 15 ms - Result correct: true
Query 29b >>             Runtime: 14 ms - Result correct: true
Query 29c >>             Runtime: 42 ms - Result correct: true
Query 30a >>             Runtime: 23 ms - Result correct: true
Query 30b >>             Runtime: 15 ms - Result correct: true
Query 30c >>             Runtime: 34 ms - Result correct: true
Query 31a >>             Runtime: 29 ms - Result correct: true
Query 31b >>             Runtime: 18 ms - Result correct: true
Query 31c >>             Runtime: 37 ms - Result correct: true
Query 32a >>             Runtime: 11 ms - Result correct: true
Query 32b >>             Runtime: 22 ms - Result correct: true
Query 33a >>             Runtime: 25 ms - Result correct: true
Query 33b >>             Runtime: 19 ms - Result correct: true
Query 33c >>             Runtime: 49 ms - Result correct: true
Total runtime: 7178 ms
```

<<<<<<< Updated upstream
=======
## Fast with Parallel Build of Hashtable
Runtime measurements captured from `./run.sh fast_plans` which includes test queries `1a-33c` had a mean total runtime of `6.8` seconds.
```bash
$ ./build.sh fast_plans
$ ./run.sh fast_plans
Query 1a >>              Runtime: 23 ms - Result correct: true
Query 1b >>              Runtime: 10 ms - Result correct: true
Query 1c >>              Runtime: 4 ms - Result correct: true
Query 1d >>              Runtime: 13 ms - Result correct: true
Query 2a >>              Runtime: 27 ms - Result correct: true
Query 2b >>              Runtime: 23 ms - Result correct: true
Query 2c >>              Runtime: 17 ms - Result correct: true
Query 2d >>              Runtime: 36 ms - Result correct: true
Query 3a >>              Runtime: 8 ms - Result correct: true
Query 3b >>              Runtime: 6 ms - Result correct: true
Query 3c >>              Runtime: 13 ms - Result correct: true
Query 4a >>              Runtime: 15 ms - Result correct: true
Query 4b >>              Runtime: 7 ms - Result correct: true
Query 4c >>              Runtime: 25 ms - Result correct: true
Query 5a >>              Runtime: 1 ms - Result correct: true
Query 5b >>              Runtime: 0 ms - Result correct: true
Query 5c >>              Runtime: 8 ms - Result correct: true
Query 6a >>              Runtime: 40 ms - Result correct: true
Query 6b >>              Runtime: 42 ms - Result correct: true
Query 6c >>              Runtime: 39 ms - Result correct: true
Query 6d >>              Runtime: 54 ms - Result correct: true
Query 6e >>              Runtime: 43 ms - Result correct: true
Query 6f >>              Runtime: 206 ms - Result correct: true
Query 7a >>              Runtime: 438 ms - Result correct: true
Query 7b >>              Runtime: 62 ms - Result correct: true
Query 7c >>              Runtime: 81 ms - Result correct: true
Query 8a >>              Runtime: 12 ms - Result correct: true
Query 8b >>              Runtime: 7 ms - Result correct: true
Query 8c >>              Runtime: 527 ms - Result correct: true
Query 8d >>              Runtime: 271 ms - Result correct: true
Query 9a >>              Runtime: 17 ms - Result correct: true
Query 9b >>              Runtime: 14 ms - Result correct: true
Query 9c >>              Runtime: 26 ms - Result correct: true
Query 9d >>              Runtime: 125 ms - Result correct: true
Query 10a >>             Runtime: 15 ms - Result correct: true
Query 10b >>             Runtime: 14 ms - Result correct: true
Query 10c >>             Runtime: 133 ms - Result correct: true
Query 11a >>             Runtime: 21 ms - Result correct: true
Query 11b >>             Runtime: 8 ms - Result correct: true
Query 11c >>             Runtime: 18 ms - Result correct: true
Query 11d >>             Runtime: 21 ms - Result correct: true
Query 12a >>             Runtime: 7 ms - Result correct: true
Query 12b >>             Runtime: 48 ms - Result correct: true
Query 12c >>             Runtime: 15 ms - Result correct: true
Query 13a >>             Runtime: 98 ms - Result correct: true
Query 13b >>             Runtime: 36 ms - Result correct: true
Query 13c >>             Runtime: 36 ms - Result correct: true
Query 13d >>             Runtime: 351 ms - Result correct: true
Query 14a >>             Runtime: 19 ms - Result correct: true
Query 14b >>             Runtime: 11 ms - Result correct: true
Query 14c >>             Runtime: 22 ms - Result correct: true
Query 15a >>             Runtime: 10 ms - Result correct: true
Query 15b >>             Runtime: 8 ms - Result correct: true
Query 15c >>             Runtime: 15 ms - Result correct: true
Query 15d >>             Runtime: 16 ms - Result correct: true
Query 16a >>             Runtime: 113 ms - Result correct: true
Query 16b >>             Runtime: 414 ms - Result correct: true
Query 16c >>             Runtime: 103 ms - Result correct: true
Query 16d >>             Runtime: 82 ms - Result correct: true
Query 17a >>             Runtime: 140 ms - Result correct: true
Query 17b >>             Runtime: 59 ms - Result correct: true
Query 17c >>             Runtime: 50 ms - Result correct: true
Query 17d >>             Runtime: 51 ms - Result correct: true
Query 17e >>             Runtime: 249 ms - Result correct: true
Query 17f >>             Runtime: 158 ms - Result correct: true
Query 18a >>             Runtime: 59 ms - Result correct: true
Query 18b >>             Runtime: 6 ms - Result correct: true
Query 18c >>             Runtime: 33 ms - Result correct: true
Query 19a >>             Runtime: 12 ms - Result correct: true
Query 19b >>             Runtime: 8 ms - Result correct: true
Query 19c >>             Runtime: 18 ms - Result correct: true
Query 19d >>             Runtime: 239 ms - Result correct: true
Query 20a >>             Runtime: 124 ms - Result correct: true
Query 20b >>             Runtime: 99 ms - Result correct: true
Query 20c >>             Runtime: 176 ms - Result correct: true
Query 21a >>             Runtime: 20 ms - Result correct: true
Query 21b >>             Runtime: 17 ms - Result correct: true
Query 21c >>             Runtime: 23 ms - Result correct: true
Query 22a >>             Runtime: 24 ms - Result correct: true
Query 22b >>             Runtime: 23 ms - Result correct: true
Query 22c >>             Runtime: 29 ms - Result correct: true
Query 22d >>             Runtime: 32 ms - Result correct: true
Query 23a >>             Runtime: 12 ms - Result correct: true
Query 23b >>             Runtime: 12 ms - Result correct: true
Query 23c >>             Runtime: 14 ms - Result correct: true
Query 24a >>             Runtime: 24 ms - Result correct: true
Query 24b >>             Runtime: 20 ms - Result correct: true
Query 25a >>             Runtime: 29 ms - Result correct: true
Query 25b >>             Runtime: 19 ms - Result correct: true
Query 25c >>             Runtime: 39 ms - Result correct: true
Query 26a >>             Runtime: 199 ms - Result correct: true
Query 26b >>             Runtime: 185 ms - Result correct: true
Query 26c >>             Runtime: 247 ms - Result correct: true
Query 27a >>             Runtime: 19 ms - Result correct: true
Query 27b >>             Runtime: 10 ms - Result correct: true
Query 27c >>             Runtime: 17 ms - Result correct: true
Query 28a >>             Runtime: 26 ms - Result correct: true
Query 28b >>             Runtime: 13 ms - Result correct: true
Query 28c >>             Runtime: 21 ms - Result correct: true
Query 29a >>             Runtime: 19 ms - Result correct: true
Query 29b >>             Runtime: 18 ms - Result correct: true
Query 29c >>             Runtime: 45 ms - Result correct: true
Query 30a >>             Runtime: 24 ms - Result correct: true
Query 30b >>             Runtime: 17 ms - Result correct: true
Query 30c >>             Runtime: 35 ms - Result correct: true
Query 31a >>             Runtime: 32 ms - Result correct: true
Query 31b >>             Runtime: 20 ms - Result correct: true
Query 31c >>             Runtime: 40 ms - Result correct: true
Query 32a >>             Runtime: 12 ms - Result correct: true
Query 32b >>             Runtime: 21 ms - Result correct: true
Query 33a >>             Runtime: 27 ms - Result correct: true
Query 33b >>             Runtime: 23 ms - Result correct: true
Query 33c >>             Runtime: 43 ms - Result correct: true
Total runtime: 6805 ms
```
>>>>>>> Stashed changes
# Benchmark conclusions

As is evident, each of the 3 optimizations led to significant speedup in our program (19.25%, 78.67%, 90.08%) respectively (measured in relation to the default runtime).