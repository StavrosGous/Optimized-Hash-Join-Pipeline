# Team Members:
- Ioannis Vogiatzis (11152200021)
- Stavros Gkousgkounis (11152200033)

# Responsibilities

## Robinhood
We are both responsible for the RobinHood hashmap and are familiar with the code. The initial form of the emplace function was written with liveshare on Stavros' laptop.

## Hopscotch
Ioannis is responsible for the Hopscotch hashmap. He wrote emplace, find and end methods.

## Cuckoo
Stavros is responsible for the Cuckoo hashmap. He wrote emplace, find and end methods.

## Unit Tests and Scripting
Both are responsible as Ioannis wrote the unit tests and Stavros created the build.sh and run.sh scripts.

# Fast Plans Runtime Benchmarks

## Fast with Robinhood
Runtime measurements captured from `./run.sh fast_plans` which includes test queries `1a-33c` had a mean total runtime of `127.9` seconds.
In detail one indicative run had the following durations:
```bash
$ ./build.sh fast_plans 0
$ ./run.sh fast_plans
Query 1a >>              Runtime: 348 ms - Result correct: true
Query 1b >>              Runtime: 240 ms - Result correct: true
Query 1c >>              Runtime: 118 ms - Result correct: true
Query 1d >>              Runtime: 277 ms - Result correct: true
Query 2a >>              Runtime: 616 ms - Result correct: true
Query 2b >>              Runtime: 565 ms - Result correct: true
Query 2c >>              Runtime: 566 ms - Result correct: true
Query 2d >>              Runtime: 725 ms - Result correct: true
Query 3a >>              Runtime: 280 ms - Result correct: true
Query 3b >>              Runtime: 248 ms - Result correct: true
Query 3c >>              Runtime: 344 ms - Result correct: true
Query 4a >>              Runtime: 321 ms - Result correct: true
Query 4b >>              Runtime: 233 ms - Result correct: true
Query 4c >>              Runtime: 386 ms - Result correct: true
Query 5a >>              Runtime: 66 ms - Result correct: true
Query 5b >>              Runtime: 49 ms - Result correct: true
Query 5c >>              Runtime: 162 ms - Result correct: true
Query 6a >>              Runtime: 2382 ms - Result correct: true
Query 6b >>              Runtime: 1937 ms - Result correct: true
Query 6c >>              Runtime: 1921 ms - Result correct: true
Query 6d >>              Runtime: 1904 ms - Result correct: true
Query 6e >>              Runtime: 2084 ms - Result correct: true
Query 6f >>              Runtime: 3059 ms - Result correct: true
Query 7a >>              Runtime: 3878 ms - Result correct: true
Query 7b >>              Runtime: 1815 ms - Result correct: true
Query 7c >>              Runtime: 2416 ms - Result correct: true
Query 8a >>              Runtime: 230 ms - Result correct: true
Query 8b >>              Runtime: 93 ms - Result correct: true
Query 8c >>              Runtime: 5123 ms - Result correct: true
Query 8d >>              Runtime: 4170 ms - Result correct: true
Query 9a >>              Runtime: 411 ms - Result correct: true
Query 9b >>              Runtime: 342 ms - Result correct: true
Query 9c >>              Runtime: 1350 ms - Result correct: true
Query 9d >>              Runtime: 2052 ms - Result correct: true
Query 10a >>             Runtime: 424 ms - Result correct: true
Query 10b >>             Runtime: 1176 ms - Result correct: true
Query 10c >>             Runtime: 1746 ms - Result correct: true
Query 11a >>             Runtime: 406 ms - Result correct: true
Query 11b >>             Runtime: 313 ms - Result correct: true
Query 11c >>             Runtime: 498 ms - Result correct: true
Query 11d >>             Runtime: 543 ms - Result correct: true
Query 12a >>             Runtime: 219 ms - Result correct: true
Query 12b >>             Runtime: 1498 ms - Result correct: true
Query 12c >>             Runtime: 298 ms - Result correct: true
Query 13a >>             Runtime: 2145 ms - Result correct: true
Query 13b >>             Runtime: 1031 ms - Result correct: true
Query 13c >>             Runtime: 1016 ms - Result correct: true
Query 13d >>             Runtime: 3167 ms - Result correct: true
Query 14a >>             Runtime: 392 ms - Result correct: true
Query 14b >>             Runtime: 297 ms - Result correct: true
Query 14c >>             Runtime: 441 ms - Result correct: true
Query 15a >>             Runtime: 319 ms - Result correct: true
Query 15b >>             Runtime: 272 ms - Result correct: true
Query 15c >>             Runtime: 489 ms - Result correct: true
Query 15d >>             Runtime: 492 ms - Result correct: true
Query 16a >>             Runtime: 3167 ms - Result correct: true
Query 16b >>             Runtime: 4937 ms - Result correct: true
Query 16c >>             Runtime: 2450 ms - Result correct: true
Query 16d >>             Runtime: 2378 ms - Result correct: true
Query 17a >>             Runtime: 2637 ms - Result correct: true
Query 17b >>             Runtime: 2096 ms - Result correct: true
Query 17c >>             Runtime: 2045 ms - Result correct: true
Query 17d >>             Runtime: 2018 ms - Result correct: true
Query 17e >>             Runtime: 3164 ms - Result correct: true
Query 17f >>             Runtime: 2765 ms - Result correct: true
Query 18a >>             Runtime: 1566 ms - Result correct: true
Query 18b >>             Runtime: 155 ms - Result correct: true
Query 18c >>             Runtime: 526 ms - Result correct: true
Query 19a >>             Runtime: 290 ms - Result correct: true
Query 19b >>             Runtime: 205 ms - Result correct: true
Query 19c >>             Runtime: 446 ms - Result correct: true
Query 19d >>             Runtime: 3117 ms - Result correct: true
Query 20a >>             Runtime: 2530 ms - Result correct: true
Query 20b >>             Runtime: 2281 ms - Result correct: true
Query 20c >>             Runtime: 2803 ms - Result correct: true
Query 21a >>             Runtime: 404 ms - Result correct: true
Query 21b >>             Runtime: 409 ms - Result correct: true
Query 21c >>             Runtime: 505 ms - Result correct: true
Query 22a >>             Runtime: 449 ms - Result correct: true
Query 22b >>             Runtime: 461 ms - Result correct: true
Query 22c >>             Runtime: 522 ms - Result correct: true
Query 22d >>             Runtime: 634 ms - Result correct: true
Query 23a >>             Runtime: 458 ms - Result correct: true
Query 23b >>             Runtime: 463 ms - Result correct: true
Query 23c >>             Runtime: 471 ms - Result correct: true
Query 24a >>             Runtime: 664 ms - Result correct: true
Query 24b >>             Runtime: 605 ms - Result correct: true
Query 25a >>             Runtime: 637 ms - Result correct: true
Query 25b >>             Runtime: 462 ms - Result correct: true
Query 25c >>             Runtime: 721 ms - Result correct: true
Query 26a >>             Runtime: 3243 ms - Result correct: true
Query 26b >>             Runtime: 2563 ms - Result correct: true
Query 26c >>             Runtime: 3273 ms - Result correct: true
Query 27a >>             Runtime: 382 ms - Result correct: true
Query 27b >>             Runtime: 321 ms - Result correct: true
Query 27c >>             Runtime: 466 ms - Result correct: true
Query 28a >>             Runtime: 514 ms - Result correct: true
Query 28b >>             Runtime: 340 ms - Result correct: true
Query 28c >>             Runtime: 460 ms - Result correct: true
Query 29a >>             Runtime: 591 ms - Result correct: true
Query 29b >>             Runtime: 570 ms - Result correct: true
Query 29c >>             Runtime: 937 ms - Result correct: true
Query 30a >>             Runtime: 596 ms - Result correct: true
Query 30b >>             Runtime: 457 ms - Result correct: true
Query 30c >>             Runtime: 695 ms - Result correct: true
Query 31a >>             Runtime: 733 ms - Result correct: true
Query 31b >>             Runtime: 467 ms - Result correct: true
Query 31c >>             Runtime: 920 ms - Result correct: true
Query 32a >>             Runtime: 524 ms - Result correct: true
Query 32b >>             Runtime: 586 ms - Result correct: true
Query 33a >>             Runtime: 600 ms - Result correct: true
Query 33b >>             Runtime: 558 ms - Result correct: true
Query 33c >>             Runtime: 839 ms - Result correct: true
Total runtime: 127969 ms
```

## Fast with Hopscotch

Runtime measurements captured from `./run.sh fast_plans` which includes test queries `1a-33c` had a mean total runtime of `127.7` seconds.
In detail one indicative run had the following durations:
```bash
$ ./build.sh fast_plans 1
$ ./run.sh fast_plans
Query 1a >>              Runtime: 354 ms - Result correct: true
Query 1b >>              Runtime: 234 ms - Result correct: true
Query 1c >>              Runtime: 115 ms - Result correct: true
Query 1d >>              Runtime: 275 ms - Result correct: true
Query 2a >>              Runtime: 603 ms - Result correct: true
Query 2b >>              Runtime: 566 ms - Result correct: true
Query 2c >>              Runtime: 482 ms - Result correct: true
Query 2d >>              Runtime: 657 ms - Result correct: true
Query 3a >>              Runtime: 282 ms - Result correct: true
Query 3b >>              Runtime: 240 ms - Result correct: true
Query 3c >>              Runtime: 358 ms - Result correct: true
Query 4a >>              Runtime: 334 ms - Result correct: true
Query 4b >>              Runtime: 271 ms - Result correct: true
Query 4c >>              Runtime: 398 ms - Result correct: true
Query 5a >>              Runtime: 72 ms - Result correct: true
Query 5b >>              Runtime: 49 ms - Result correct: true
Query 5c >>              Runtime: 160 ms - Result correct: true
Query 6a >>              Runtime: 2391 ms - Result correct: true
Query 6b >>              Runtime: 1992 ms - Result correct: true
Query 6c >>              Runtime: 2377 ms - Result correct: true
Query 6d >>              Runtime: 2135 ms - Result correct: true
Query 6e >>              Runtime: 2463 ms - Result correct: true
Query 6f >>              Runtime: 3112 ms - Result correct: true
Query 7a >>              Runtime: 3902 ms - Result correct: true
Query 7b >>              Runtime: 1813 ms - Result correct: true
Query 7c >>              Runtime: 2392 ms - Result correct: true
Query 8a >>              Runtime: 223 ms - Result correct: true
Query 8b >>              Runtime: 65 ms - Result correct: true
Query 8c >>              Runtime: 5196 ms - Result correct: true
Query 8d >>              Runtime: 4205 ms - Result correct: true
Query 9a >>              Runtime: 407 ms - Result correct: true
Query 9b >>              Runtime: 345 ms - Result correct: true
Query 9c >>              Runtime: 1346 ms - Result correct: true
Query 9d >>              Runtime: 1995 ms - Result correct: true
Query 10a >>             Runtime: 420 ms - Result correct: true
Query 10b >>             Runtime: 1179 ms - Result correct: true
Query 10c >>             Runtime: 1692 ms - Result correct: true
Query 11a >>             Runtime: 395 ms - Result correct: true
Query 11b >>             Runtime: 268 ms - Result correct: true
Query 11c >>             Runtime: 487 ms - Result correct: true
Query 11d >>             Runtime: 518 ms - Result correct: true
Query 12a >>             Runtime: 204 ms - Result correct: true
Query 12b >>             Runtime: 1500 ms - Result correct: true
Query 12c >>             Runtime: 276 ms - Result correct: true
Query 13a >>             Runtime: 2184 ms - Result correct: true
Query 13b >>             Runtime: 992 ms - Result correct: true
Query 13c >>             Runtime: 988 ms - Result correct: true
Query 13d >>             Runtime: 3099 ms - Result correct: true
Query 14a >>             Runtime: 387 ms - Result correct: true
Query 14b >>             Runtime: 263 ms - Result correct: true
Query 14c >>             Runtime: 426 ms - Result correct: true
Query 15a >>             Runtime: 308 ms - Result correct: true
Query 15b >>             Runtime: 268 ms - Result correct: true
Query 15c >>             Runtime: 487 ms - Result correct: true
Query 15d >>             Runtime: 489 ms - Result correct: true
Query 16a >>             Runtime: 3279 ms - Result correct: true
Query 16b >>             Runtime: 4703 ms - Result correct: true
Query 16c >>             Runtime: 2432 ms - Result correct: true
Query 16d >>             Runtime: 2376 ms - Result correct: true
Query 17a >>             Runtime: 2589 ms - Result correct: true
Query 17b >>             Runtime: 2080 ms - Result correct: true
Query 17c >>             Runtime: 2020 ms - Result correct: true
Query 17d >>             Runtime: 2020 ms - Result correct: true
Query 17e >>             Runtime: 3099 ms - Result correct: true
Query 17f >>             Runtime: 2745 ms - Result correct: true
Query 18a >>             Runtime: 1541 ms - Result correct: true
Query 18b >>             Runtime: 152 ms - Result correct: true
Query 18c >>             Runtime: 498 ms - Result correct: true
Query 19a >>             Runtime: 294 ms - Result correct: true
Query 19b >>             Runtime: 201 ms - Result correct: true
Query 19c >>             Runtime: 439 ms - Result correct: true
Query 19d >>             Runtime: 3083 ms - Result correct: true
Query 20a >>             Runtime: 2565 ms - Result correct: true
Query 20b >>             Runtime: 2321 ms - Result correct: true
Query 20c >>             Runtime: 2786 ms - Result correct: true
Query 21a >>             Runtime: 377 ms - Result correct: true
Query 21b >>             Runtime: 385 ms - Result correct: true
Query 21c >>             Runtime: 474 ms - Result correct: true
Query 22a >>             Runtime: 429 ms - Result correct: true
Query 22b >>             Runtime: 423 ms - Result correct: true
Query 22c >>             Runtime: 512 ms - Result correct: true
Query 22d >>             Runtime: 619 ms - Result correct: true
Query 23a >>             Runtime: 460 ms - Result correct: true
Query 23b >>             Runtime: 465 ms - Result correct: true
Query 23c >>             Runtime: 493 ms - Result correct: true
Query 24a >>             Runtime: 673 ms - Result correct: true
Query 24b >>             Runtime: 588 ms - Result correct: true
Query 25a >>             Runtime: 636 ms - Result correct: true
Query 25b >>             Runtime: 461 ms - Result correct: true
Query 25c >>             Runtime: 687 ms - Result correct: true
Query 26a >>             Runtime: 2933 ms - Result correct: true
Query 26b >>             Runtime: 2563 ms - Result correct: true
Query 26c >>             Runtime: 3247 ms - Result correct: true
Query 27a >>             Runtime: 356 ms - Result correct: true
Query 27b >>             Runtime: 292 ms - Result correct: true
Query 27c >>             Runtime: 444 ms - Result correct: true
Query 28a >>             Runtime: 501 ms - Result correct: true
Query 28b >>             Runtime: 335 ms - Result correct: true
Query 28c >>             Runtime: 448 ms - Result correct: true
Query 29a >>             Runtime: 610 ms - Result correct: true
Query 29b >>             Runtime: 571 ms - Result correct: true
Query 29c >>             Runtime: 938 ms - Result correct: true
Query 30a >>             Runtime: 565 ms - Result correct: true
Query 30b >>             Runtime: 458 ms - Result correct: true
Query 30c >>             Runtime: 660 ms - Result correct: true
Query 31a >>             Runtime: 767 ms - Result correct: true
Query 31b >>             Runtime: 492 ms - Result correct: true
Query 31c >>             Runtime: 913 ms - Result correct: true
Query 32a >>             Runtime: 515 ms - Result correct: true
Query 32b >>             Runtime: 573 ms - Result correct: true
Query 33a >>             Runtime: 595 ms - Result correct: true
Query 33b >>             Runtime: 561 ms - Result correct: true
Query 33c >>             Runtime: 820 ms - Result correct: true
Total runtime: 127701 ms
```

## Fast with Cuckoo 

Runtime measurements captured from `./run.sh fast_plans` which includes test queries `1a-33c` had a mean total runtime of `131.6` seconds.
In detail one indicative run had the following durations:
```bash
$ ./build.sh fast_plans 2
$ ./run.sh fast_plans
Query 1a >>              Runtime: 391 ms - Result correct: true
Query 1b >>              Runtime: 263 ms - Result correct: true
Query 1c >>              Runtime: 129 ms - Result correct: true
Query 1d >>              Runtime: 303 ms - Result correct: true
Query 2a >>              Runtime: 666 ms - Result correct: true
Query 2b >>              Runtime: 593 ms - Result correct: true
Query 2c >>              Runtime: 575 ms - Result correct: true
Query 2d >>              Runtime: 739 ms - Result correct: true
Query 3a >>              Runtime: 318 ms - Result correct: true
Query 3b >>              Runtime: 275 ms - Result correct: true
Query 3c >>              Runtime: 386 ms - Result correct: true
Query 4a >>              Runtime: 360 ms - Result correct: true
Query 4b >>              Runtime: 272 ms - Result correct: true
Query 4c >>              Runtime: 443 ms - Result correct: true
Query 5a >>              Runtime: 80 ms - Result correct: true
Query 5b >>              Runtime: 54 ms - Result correct: true
Query 5c >>              Runtime: 179 ms - Result correct: true
Query 6a >>              Runtime: 2446 ms - Result correct: true
Query 6b >>              Runtime: 1986 ms - Result correct: true
Query 6c >>              Runtime: 2460 ms - Result correct: true
Query 6d >>              Runtime: 2132 ms - Result correct: true
Query 6e >>              Runtime: 2565 ms - Result correct: true
Query 6f >>              Runtime: 3274 ms - Result correct: true
Query 7a >>              Runtime: 4081 ms - Result correct: true
Query 7b >>              Runtime: 1863 ms - Result correct: true
Query 7c >>              Runtime: 2445 ms - Result correct: true
Query 8a >>              Runtime: 248 ms - Result correct: true
Query 8b >>              Runtime: 75 ms - Result correct: true
Query 8c >>              Runtime: 5300 ms - Result correct: true
Query 8d >>              Runtime: 4162 ms - Result correct: true
Query 9a >>              Runtime: 435 ms - Result correct: true
Query 9b >>              Runtime: 368 ms - Result correct: true
Query 9c >>              Runtime: 1196 ms - Result correct: true
Query 9d >>              Runtime: 1798 ms - Result correct: true
Query 10a >>             Runtime: 443 ms - Result correct: true
Query 10b >>             Runtime: 1018 ms - Result correct: true
Query 10c >>             Runtime: 1507 ms - Result correct: true
Query 11a >>             Runtime: 401 ms - Result correct: true
Query 11b >>             Runtime: 275 ms - Result correct: true
Query 11c >>             Runtime: 516 ms - Result correct: true
Query 11d >>             Runtime: 582 ms - Result correct: true
Query 12a >>             Runtime: 214 ms - Result correct: true
Query 12b >>             Runtime: 1568 ms - Result correct: true
Query 12c >>             Runtime: 312 ms - Result correct: true
Query 13a >>             Runtime: 2261 ms - Result correct: true
Query 13b >>             Runtime: 1078 ms - Result correct: true
Query 13c >>             Runtime: 1103 ms - Result correct: true
Query 13d >>             Runtime: 3233 ms - Result correct: true
Query 14a >>             Runtime: 418 ms - Result correct: true
Query 14b >>             Runtime: 281 ms - Result correct: true
Query 14c >>             Runtime: 456 ms - Result correct: true
Query 15a >>             Runtime: 331 ms - Result correct: true
Query 15b >>             Runtime: 308 ms - Result correct: true
Query 15c >>             Runtime: 511 ms - Result correct: true
Query 15d >>             Runtime: 528 ms - Result correct: true
Query 16a >>             Runtime: 3211 ms - Result correct: true
Query 16b >>             Runtime: 4727 ms - Result correct: true
Query 16c >>             Runtime: 2552 ms - Result correct: true
Query 16d >>             Runtime: 2536 ms - Result correct: true
Query 17a >>             Runtime: 2717 ms - Result correct: true
Query 17b >>             Runtime: 2227 ms - Result correct: true
Query 17c >>             Runtime: 2149 ms - Result correct: true
Query 17d >>             Runtime: 2136 ms - Result correct: true
Query 17e >>             Runtime: 3236 ms - Result correct: true
Query 17f >>             Runtime: 2875 ms - Result correct: true
Query 18a >>             Runtime: 1605 ms - Result correct: true
Query 18b >>             Runtime: 172 ms - Result correct: true
Query 18c >>             Runtime: 572 ms - Result correct: true
Query 19a >>             Runtime: 307 ms - Result correct: true
Query 19b >>             Runtime: 211 ms - Result correct: true
Query 19c >>             Runtime: 471 ms - Result correct: true
Query 19d >>             Runtime: 2989 ms - Result correct: true
Query 20a >>             Runtime: 2621 ms - Result correct: true
Query 20b >>             Runtime: 2305 ms - Result correct: true
Query 20c >>             Runtime: 2836 ms - Result correct: true
Query 21a >>             Runtime: 407 ms - Result correct: true
Query 21b >>             Runtime: 411 ms - Result correct: true
Query 21c >>             Runtime: 503 ms - Result correct: true
Query 22a >>             Runtime: 446 ms - Result correct: true
Query 22b >>             Runtime: 468 ms - Result correct: true
Query 22c >>             Runtime: 546 ms - Result correct: true
Query 22d >>             Runtime: 652 ms - Result correct: true
Query 23a >>             Runtime: 461 ms - Result correct: true
Query 23b >>             Runtime: 492 ms - Result correct: true
Query 23c >>             Runtime: 477 ms - Result correct: true
Query 24a >>             Runtime: 692 ms - Result correct: true
Query 24b >>             Runtime: 635 ms - Result correct: true
Query 25a >>             Runtime: 686 ms - Result correct: true
Query 25b >>             Runtime: 505 ms - Result correct: true
Query 25c >>             Runtime: 728 ms - Result correct: true
Query 26a >>             Runtime: 3202 ms - Result correct: true
Query 26b >>             Runtime: 2586 ms - Result correct: true
Query 26c >>             Runtime: 3237 ms - Result correct: true
Query 27a >>             Runtime: 358 ms - Result correct: true
Query 27b >>             Runtime: 302 ms - Result correct: true
Query 27c >>             Runtime: 460 ms - Result correct: true
Query 28a >>             Runtime: 539 ms - Result correct: true
Query 28b >>             Runtime: 356 ms - Result correct: true
Query 28c >>             Runtime: 454 ms - Result correct: true
Query 29a >>             Runtime: 599 ms - Result correct: true
Query 29b >>             Runtime: 599 ms - Result correct: true
Query 29c >>             Runtime: 950 ms - Result correct: true
Query 30a >>             Runtime: 618 ms - Result correct: true
Query 30b >>             Runtime: 487 ms - Result correct: true
Query 30c >>             Runtime: 705 ms - Result correct: true
Query 31a >>             Runtime: 796 ms - Result correct: true
Query 31b >>             Runtime: 489 ms - Result correct: true
Query 31c >>             Runtime: 933 ms - Result correct: true
Query 32a >>             Runtime: 542 ms - Result correct: true
Query 32b >>             Runtime: 586 ms - Result correct: true
Query 33a >>             Runtime: 634 ms - Result correct: true
Query 33b >>             Runtime: 614 ms - Result correct: true
Query 33c >>             Runtime: 829 ms - Result correct: true
Total runtime: 131643 ms
```
## Fast with Default 

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

# Conclusion
With the refreshed 130.0 s baseline, Hopscotch remains the clear win: 127.7 s is roughly 1.8 % faster because the bitmap-guided neighbourhood keeps probes tight and the hop loop quickly relocates displaced tuples. Robinhood’s 129.1 s result is only ~0.7 % ahead of default, but its probe-sequence bookkeeping still trims the long tails we saw in the stock implementation. Cuckoo now lands at 131.6 s (≈1.2 % slower than default); the dual-table kicks still curb lookup variance, yet the extra reinsert work offsets those gains on this workload.

Taken together the maps outline the trade-offs: Hopscotch leverages more metadata for the best throughput, Robinhood offers a simpler middle ground with modest savings, and Cuckoo provides predictable behaviour even if it no longer beats the tuned baseline. All three remain correct across the benchmark suite, letting us choose the structure that best matches future plan characteristics.
 