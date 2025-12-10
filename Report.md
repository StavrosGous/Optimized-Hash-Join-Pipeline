# Team Members:
- Ioannis Vogiatzis (11152200021)
- Stavros Gkousgkounis (11152200033)

# Responsibilities
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

## Fast, Late Materialization
```bash
$ ./build.sh fast_plans 1 # We still use the num 1 for hopscotch
$ ./run.sh fast_plans
Query 1a >>              Runtime: 218 ms - Result correct: true
Query 1b >>              Runtime: 159 ms - Result correct: true
Query 1c >>              Runtime: 72 ms - Result correct: true
Query 1d >>              Runtime: 209 ms - Result correct: true
Query 2a >>              Runtime: 444 ms - Result correct: true
Query 2b >>              Runtime: 412 ms - Result correct: true
Query 2c >>              Runtime: 381 ms - Result correct: true
Query 2d >>              Runtime: 472 ms - Result correct: true
Query 3a >>              Runtime: 243 ms - Result correct: true
Query 3b >>              Runtime: 215 ms - Result correct: true
Query 3c >>              Runtime: 297 ms - Result correct: true
Query 4a >>              Runtime: 298 ms - Result correct: true
Query 4b >>              Runtime: 214 ms - Result correct: true
Query 4c >>              Runtime: 332 ms - Result correct: true
Query 5a >>              Runtime: 48 ms - Result correct: true
Query 5b >>              Runtime: 36 ms - Result correct: true
Query 5c >>              Runtime: 116 ms - Result correct: true
Query 6a >>              Runtime: 1794 ms - Result correct: true
Query 6b >>              Runtime: 1721 ms - Result correct: true
Query 6c >>              Runtime: 1850 ms - Result correct: true
Query 6d >>              Runtime: 1949 ms - Result correct: true
Query 6e >>              Runtime: 1943 ms - Result correct: true
Query 6f >>              Runtime: 2472 ms - Result correct: true
Query 7a >>              Runtime: 3015 ms - Result correct: true
Query 7b >>              Runtime: 1740 ms - Result correct: true
Query 7c >>              Runtime: 1756 ms - Result correct: true
Query 8a >>              Runtime: 153 ms - Result correct: true
Query 8b >>              Runtime: 45 ms - Result correct: true
Query 8c >>              Runtime: 3644 ms - Result correct: true
Query 8d >>              Runtime: 3070 ms - Result correct: true
Query 9a >>              Runtime: 289 ms - Result correct: true
Query 9b >>              Runtime: 238 ms - Result correct: true
Query 9c >>              Runtime: 1179 ms - Result correct: true
Query 9d >>              Runtime: 1572 ms - Result correct: true
Query 10a >>             Runtime: 303 ms - Result correct: true
Query 10b >>             Runtime: 1076 ms - Result correct: true
Query 10c >>             Runtime: 1490 ms - Result correct: true
Query 11a >>             Runtime: 305 ms - Result correct: true
Query 11b >>             Runtime: 245 ms - Result correct: true
Query 11c >>             Runtime: 367 ms - Result correct: true
Query 11d >>             Runtime: 387 ms - Result correct: true
Query 12a >>             Runtime: 179 ms - Result correct: true
Query 12b >>             Runtime: 999 ms - Result correct: true
Query 12c >>             Runtime: 240 ms - Result correct: true
Query 13a >>             Runtime: 1455 ms - Result correct: true
Query 13b >>             Runtime: 936 ms - Result correct: true
Query 13c >>             Runtime: 921 ms - Result correct: true
Query 13d >>             Runtime: 2298 ms - Result correct: true
Query 14a >>             Runtime: 342 ms - Result correct: true
Query 14b >>             Runtime: 237 ms - Result correct: true
Query 14c >>             Runtime: 374 ms - Result correct: true
Query 15a >>             Runtime: 266 ms - Result correct: true
Query 15b >>             Runtime: 236 ms - Result correct: true
Query 15c >>             Runtime: 419 ms - Result correct: true
Query 15d >>             Runtime: 417 ms - Result correct: true
Query 16a >>             Runtime: 2599 ms - Result correct: true
Query 16b >>             Runtime: 3438 ms - Result correct: true
Query 16c >>             Runtime: 2145 ms - Result correct: true
Query 16d >>             Runtime: 2100 ms - Result correct: true
Query 17a >>             Runtime: 2197 ms - Result correct: true
Query 17b >>             Runtime: 1855 ms - Result correct: true
Query 17c >>             Runtime: 1975 ms - Result correct: true
Query 17d >>             Runtime: 1832 ms - Result correct: true
Query 17e >>             Runtime: 2636 ms - Result correct: true
Query 17f >>             Runtime: 2447 ms - Result correct: true
Query 18a >>             Runtime: 988 ms - Result correct: true
Query 18b >>             Runtime: 128 ms - Result correct: true
Query 18c >>             Runtime: 411 ms - Result correct: true
Query 19a >>             Runtime: 275 ms - Result correct: true
Query 19b >>             Runtime: 195 ms - Result correct: true
Query 19c >>             Runtime: 405 ms - Result correct: true
Query 19d >>             Runtime: 2334 ms - Result correct: true
Query 20a >>             Runtime: 2399 ms - Result correct: true
Query 20b >>             Runtime: 2098 ms - Result correct: true
Query 20c >>             Runtime: 2512 ms - Result correct: true
Query 21a >>             Runtime: 306 ms - Result correct: true
Query 21b >>             Runtime: 309 ms - Result correct: true
Query 21c >>             Runtime: 384 ms - Result correct: true
Query 22a >>             Runtime: 362 ms - Result correct: true
Query 22b >>             Runtime: 345 ms - Result correct: true
Query 22c >>             Runtime: 391 ms - Result correct: true
Query 22d >>             Runtime: 506 ms - Result correct: true
Query 23a >>             Runtime: 371 ms - Result correct: true
Query 23b >>             Runtime: 370 ms - Result correct: true
Query 23c >>             Runtime: 387 ms - Result correct: true
Query 24a >>             Runtime: 523 ms - Result correct: true
Query 24b >>             Runtime: 499 ms - Result correct: true
Query 25a >>             Runtime: 490 ms - Result correct: true
Query 25b >>             Runtime: 379 ms - Result correct: true
Query 25c >>             Runtime: 523 ms - Result correct: true
Query 26a >>             Runtime: 2637 ms - Result correct: true
Query 26b >>             Runtime: 3027 ms - Result correct: true
Query 26c >>             Runtime: 2850 ms - Result correct: true
Query 27a >>             Runtime: 299 ms - Result correct: true
Query 27b >>             Runtime: 272 ms - Result correct: true
Query 27c >>             Runtime: 376 ms - Result correct: true
Query 28a >>             Runtime: 393 ms - Result correct: true
Query 28b >>             Runtime: 282 ms - Result correct: true
Query 28c >>             Runtime: 364 ms - Result correct: true
Query 29a >>             Runtime: 524 ms - Result correct: true
Query 29b >>             Runtime: 517 ms - Result correct: true
Query 29c >>             Runtime: 763 ms - Result correct: true
Query 30a >>             Runtime: 455 ms - Result correct: true
Query 30b >>             Runtime: 409 ms - Result correct: true
Query 30c >>             Runtime: 525 ms - Result correct: true
Query 31a >>             Runtime: 586 ms - Result correct: true
Query 31b >>             Runtime: 415 ms - Result correct: true
Query 31c >>             Runtime: 712 ms - Result correct: true
Query 32a >>             Runtime: 377 ms - Result correct: true
Query 32b >>             Runtime: 394 ms - Result correct: true
Query 33a >>             Runtime: 482 ms - Result correct: true
Query 33b >>             Runtime: 471 ms - Result correct: true
Query 33c >>             Runtime: 705 ms - Result correct: true
Total runtime: 105037 ms
```

## Fast, Column-Store

# Conclusion

 