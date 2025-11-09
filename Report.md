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

Runtime measurements captured from `./run.sh fast_plans` which includes test queries `1a-33c` had a mean total runtime of `126.2` seconds.
In detail one indicative run had the following durations:
```bash
$ ./build.sh fast_plans 1
$ ./run.sh fast_plans
Query 1a >>              Runtime: 374 ms - Result correct: true
Query 1b >>              Runtime: 245 ms - Result correct: true
Query 1c >>              Runtime: 126 ms - Result correct: true
Query 1d >>              Runtime: 297 ms - Result correct: true
Query 2a >>              Runtime: 639 ms - Result correct: true
Query 2b >>              Runtime: 591 ms - Result correct: true
Query 2c >>              Runtime: 518 ms - Result correct: true
Query 2d >>              Runtime: 687 ms - Result correct: true
Query 3a >>              Runtime: 303 ms - Result correct: true
Query 3b >>              Runtime: 271 ms - Result correct: true
Query 3c >>              Runtime: 385 ms - Result correct: true
Query 4a >>              Runtime: 354 ms - Result correct: true
Query 4b >>              Runtime: 266 ms - Result correct: true
Query 4c >>              Runtime: 417 ms - Result correct: true
Query 5a >>              Runtime: 78 ms - Result correct: true
Query 5b >>              Runtime: 53 ms - Result correct: true
Query 5c >>              Runtime: 171 ms - Result correct: true
Query 6a >>              Runtime: 2423 ms - Result correct: true
Query 6b >>              Runtime: 1992 ms - Result correct: true
Query 6c >>              Runtime: 2010 ms - Result correct: true
Query 6d >>              Runtime: 1949 ms - Result correct: true
Query 6e >>              Runtime: 1925 ms - Result correct: true
Query 6f >>              Runtime: 2998 ms - Result correct: true
Query 7a >>              Runtime: 4058 ms - Result correct: true
Query 7b >>              Runtime: 1833 ms - Result correct: true
Query 7c >>              Runtime: 2409 ms - Result correct: true
Query 8a >>              Runtime: 236 ms - Result correct: true
Query 8b >>              Runtime: 69 ms - Result correct: true
Query 8c >>              Runtime: 4845 ms - Result correct: true
Query 8d >>              Runtime: 3810 ms - Result correct: true
Query 9a >>              Runtime: 400 ms - Result correct: true
Query 9b >>              Runtime: 354 ms - Result correct: true
Query 9c >>              Runtime: 1330 ms - Result correct: true
Query 9d >>              Runtime: 2005 ms - Result correct: true
Query 10a >>             Runtime: 425 ms - Result correct: true
Query 10b >>             Runtime: 1186 ms - Result correct: true
Query 10c >>             Runtime: 1723 ms - Result correct: true
Query 11a >>             Runtime: 401 ms - Result correct: true
Query 11b >>             Runtime: 283 ms - Result correct: true
Query 11c >>             Runtime: 495 ms - Result correct: true
Query 11d >>             Runtime: 508 ms - Result correct: true
Query 12a >>             Runtime: 202 ms - Result correct: true
Query 12b >>             Runtime: 1530 ms - Result correct: true
Query 12c >>             Runtime: 307 ms - Result correct: true
Query 13a >>             Runtime: 2138 ms - Result correct: true
Query 13b >>             Runtime: 1022 ms - Result correct: true
Query 13c >>             Runtime: 1022 ms - Result correct: true
Query 13d >>             Runtime: 3079 ms - Result correct: true
Query 14a >>             Runtime: 388 ms - Result correct: true
Query 14b >>             Runtime: 275 ms - Result correct: true
Query 14c >>             Runtime: 446 ms - Result correct: true
Query 15a >>             Runtime: 310 ms - Result correct: true
Query 15b >>             Runtime: 272 ms - Result correct: true
Query 15c >>             Runtime: 491 ms - Result correct: true
Query 15d >>             Runtime: 495 ms - Result correct: true
Query 16a >>             Runtime: 2892 ms - Result correct: true
Query 16b >>             Runtime: 4690 ms - Result correct: true
Query 16c >>             Runtime: 2430 ms - Result correct: true
Query 16d >>             Runtime: 2368 ms - Result correct: true
Query 17a >>             Runtime: 2680 ms - Result correct: true
Query 17b >>             Runtime: 2124 ms - Result correct: true
Query 17c >>             Runtime: 2040 ms - Result correct: true
Query 17d >>             Runtime: 2108 ms - Result correct: true
Query 17e >>             Runtime: 3200 ms - Result correct: true
Query 17f >>             Runtime: 2765 ms - Result correct: true
Query 18a >>             Runtime: 1577 ms - Result correct: true
Query 18b >>             Runtime: 153 ms - Result correct: true
Query 18c >>             Runtime: 510 ms - Result correct: true
Query 19a >>             Runtime: 293 ms - Result correct: true
Query 19b >>             Runtime: 197 ms - Result correct: true
Query 19c >>             Runtime: 445 ms - Result correct: true
Query 19d >>             Runtime: 3100 ms - Result correct: true
Query 20a >>             Runtime: 2574 ms - Result correct: true
Query 20b >>             Runtime: 2274 ms - Result correct: true
Query 20c >>             Runtime: 2798 ms - Result correct: true
Query 21a >>             Runtime: 383 ms - Result correct: true
Query 21b >>             Runtime: 399 ms - Result correct: true
Query 21c >>             Runtime: 481 ms - Result correct: true
Query 22a >>             Runtime: 424 ms - Result correct: true
Query 22b >>             Runtime: 413 ms - Result correct: true
Query 22c >>             Runtime: 491 ms - Result correct: true
Query 22d >>             Runtime: 588 ms - Result correct: true
Query 23a >>             Runtime: 437 ms - Result correct: true
Query 23b >>             Runtime: 437 ms - Result correct: true
Query 23c >>             Runtime: 464 ms - Result correct: true
Query 24a >>             Runtime: 624 ms - Result correct: true
Query 24b >>             Runtime: 615 ms - Result correct: true
Query 25a >>             Runtime: 616 ms - Result correct: true
Query 25b >>             Runtime: 451 ms - Result correct: true
Query 25c >>             Runtime: 682 ms - Result correct: true
Query 26a >>             Runtime: 3170 ms - Result correct: true
Query 26b >>             Runtime: 2610 ms - Result correct: true
Query 26c >>             Runtime: 3228 ms - Result correct: true
Query 27a >>             Runtime: 341 ms - Result correct: true
Query 27b >>             Runtime: 293 ms - Result correct: true
Query 27c >>             Runtime: 435 ms - Result correct: true
Query 28a >>             Runtime: 502 ms - Result correct: true
Query 28b >>             Runtime: 344 ms - Result correct: true
Query 28c >>             Runtime: 424 ms - Result correct: true
Query 29a >>             Runtime: 562 ms - Result correct: true
Query 29b >>             Runtime: 558 ms - Result correct: true
Query 29c >>             Runtime: 894 ms - Result correct: true
Query 30a >>             Runtime: 527 ms - Result correct: true
Query 30b >>             Runtime: 450 ms - Result correct: true
Query 30c >>             Runtime: 644 ms - Result correct: true
Query 31a >>             Runtime: 733 ms - Result correct: true
Query 31b >>             Runtime: 455 ms - Result correct: true
Query 31c >>             Runtime: 888 ms - Result correct: true
Query 32a >>             Runtime: 514 ms - Result correct: true
Query 32b >>             Runtime: 554 ms - Result correct: true
Query 33a >>             Runtime: 601 ms - Result correct: true
Query 33b >>             Runtime: 551 ms - Result correct: true
Query 33c >>             Runtime: 814 ms - Result correct: true
Total runtime: 126236 ms
```

## Fast with Cuckoo 

Runtime measurements captured from `./run.sh fast_plans` which includes test queries `1a-33c` had a mean total runtime of `127.8` seconds.
In detail one indicative run had the following durations:
```bash
$ ./build.sh fast_plans 2
$ ./run.sh fast_plans
Query 1a >>              Runtime: 369 ms - Result correct: true
Query 1b >>              Runtime: 249 ms - Result correct: true
Query 1c >>              Runtime: 127 ms - Result correct: true
Query 1d >>              Runtime: 292 ms - Result correct: true
Query 2a >>              Runtime: 610 ms - Result correct: true
Query 2b >>              Runtime: 574 ms - Result correct: true
Query 2c >>              Runtime: 578 ms - Result correct: true
Query 2d >>              Runtime: 710 ms - Result correct: true
Query 3a >>              Runtime: 294 ms - Result correct: true
Query 3b >>              Runtime: 251 ms - Result correct: true
Query 3c >>              Runtime: 359 ms - Result correct: true
Query 4a >>              Runtime: 334 ms - Result correct: true
Query 4b >>              Runtime: 249 ms - Result correct: true
Query 4c >>              Runtime: 406 ms - Result correct: true
Query 5a >>              Runtime: 68 ms - Result correct: true
Query 5b >>              Runtime: 51 ms - Result correct: true
Query 5c >>              Runtime: 167 ms - Result correct: true
Query 6a >>              Runtime: 2407 ms - Result correct: true
Query 6b >>              Runtime: 2009 ms - Result correct: true
Query 6c >>              Runtime: 2377 ms - Result correct: true
Query 6d >>              Runtime: 2117 ms - Result correct: true
Query 6e >>              Runtime: 2505 ms - Result correct: true
Query 6f >>              Runtime: 3193 ms - Result correct: true
Query 7a >>              Runtime: 4018 ms - Result correct: true
Query 7b >>              Runtime: 1822 ms - Result correct: true
Query 7c >>              Runtime: 2393 ms - Result correct: true
Query 8a >>              Runtime: 227 ms - Result correct: true
Query 8b >>              Runtime: 68 ms - Result correct: true
Query 8c >>              Runtime: 4973 ms - Result correct: true
Query 8d >>              Runtime: 3961 ms - Result correct: true
Query 9a >>              Runtime: 436 ms - Result correct: true
Query 9b >>              Runtime: 368 ms - Result correct: true
Query 9c >>              Runtime: 1150 ms - Result correct: true
Query 9d >>              Runtime: 1797 ms - Result correct: true
Query 10a >>             Runtime: 454 ms - Result correct: true
Query 10b >>             Runtime: 982 ms - Result correct: true
Query 10c >>             Runtime: 1506 ms - Result correct: true
Query 11a >>             Runtime: 400 ms - Result correct: true
Query 11b >>             Runtime: 307 ms - Result correct: true
Query 11c >>             Runtime: 525 ms - Result correct: true
Query 11d >>             Runtime: 572 ms - Result correct: true
Query 12a >>             Runtime: 209 ms - Result correct: true
Query 12b >>             Runtime: 1577 ms - Result correct: true
Query 12c >>             Runtime: 286 ms - Result correct: true
Query 13a >>             Runtime: 2095 ms - Result correct: true
Query 13b >>             Runtime: 1041 ms - Result correct: true
Query 13c >>             Runtime: 1023 ms - Result correct: true
Query 13d >>             Runtime: 3169 ms - Result correct: true
Query 14a >>             Runtime: 375 ms - Result correct: true
Query 14b >>             Runtime: 260 ms - Result correct: true
Query 14c >>             Runtime: 420 ms - Result correct: true
Query 15a >>             Runtime: 315 ms - Result correct: true
Query 15b >>             Runtime: 293 ms - Result correct: true
Query 15c >>             Runtime: 470 ms - Result correct: true
Query 15d >>             Runtime: 495 ms - Result correct: true
Query 16a >>             Runtime: 2887 ms - Result correct: true
Query 16b >>             Runtime: 4735 ms - Result correct: true
Query 16c >>             Runtime: 2501 ms - Result correct: true
Query 16d >>             Runtime: 2563 ms - Result correct: true
Query 17a >>             Runtime: 2661 ms - Result correct: true
Query 17b >>             Runtime: 2136 ms - Result correct: true
Query 17c >>             Runtime: 2043 ms - Result correct: true
Query 17d >>             Runtime: 2081 ms - Result correct: true
Query 17e >>             Runtime: 3154 ms - Result correct: true
Query 17f >>             Runtime: 2735 ms - Result correct: true
Query 18a >>             Runtime: 1573 ms - Result correct: true
Query 18b >>             Runtime: 166 ms - Result correct: true
Query 18c >>             Runtime: 559 ms - Result correct: true
Query 19a >>             Runtime: 299 ms - Result correct: true
Query 19b >>             Runtime: 211 ms - Result correct: true
Query 19c >>             Runtime: 451 ms - Result correct: true
Query 19d >>             Runtime: 2934 ms - Result correct: true
Query 20a >>             Runtime: 2629 ms - Result correct: true
Query 20b >>             Runtime: 2237 ms - Result correct: true
Query 20c >>             Runtime: 2893 ms - Result correct: true
Query 21a >>             Runtime: 393 ms - Result correct: true
Query 21b >>             Runtime: 399 ms - Result correct: true
Query 21c >>             Runtime: 484 ms - Result correct: true
Query 22a >>             Runtime: 442 ms - Result correct: true
Query 22b >>             Runtime: 415 ms - Result correct: true
Query 22c >>             Runtime: 481 ms - Result correct: true
Query 22d >>             Runtime: 608 ms - Result correct: true
Query 23a >>             Runtime: 440 ms - Result correct: true
Query 23b >>             Runtime: 449 ms - Result correct: true
Query 23c >>             Runtime: 480 ms - Result correct: true
Query 24a >>             Runtime: 655 ms - Result correct: true
Query 24b >>             Runtime: 646 ms - Result correct: true
Query 25a >>             Runtime: 623 ms - Result correct: true
Query 25b >>             Runtime: 468 ms - Result correct: true
Query 25c >>             Runtime: 716 ms - Result correct: true
Query 26a >>             Runtime: 3191 ms - Result correct: true
Query 26b >>             Runtime: 2618 ms - Result correct: true
Query 26c >>             Runtime: 3331 ms - Result correct: true
Query 27a >>             Runtime: 347 ms - Result correct: true
Query 27b >>             Runtime: 291 ms - Result correct: true
Query 27c >>             Runtime: 455 ms - Result correct: true
Query 28a >>             Runtime: 506 ms - Result correct: true
Query 28b >>             Runtime: 333 ms - Result correct: true
Query 28c >>             Runtime: 441 ms - Result correct: true
Query 29a >>             Runtime: 574 ms - Result correct: true
Query 29b >>             Runtime: 573 ms - Result correct: true
Query 29c >>             Runtime: 916 ms - Result correct: true
Query 30a >>             Runtime: 546 ms - Result correct: true
Query 30b >>             Runtime: 471 ms - Result correct: true
Query 30c >>             Runtime: 676 ms - Result correct: true
Query 31a >>             Runtime: 766 ms - Result correct: true
Query 31b >>             Runtime: 459 ms - Result correct: true
Query 31c >>             Runtime: 908 ms - Result correct: true
Query 32a >>             Runtime: 518 ms - Result correct: true
Query 32b >>             Runtime: 525 ms - Result correct: true
Query 33a >>             Runtime: 621 ms - Result correct: true
Query 33b >>             Runtime: 578 ms - Result correct: true
Query 33c >>             Runtime: 785 ms - Result correct: true
Total runtime: 127860 ms
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
All three hash maps use the same 70 % load-trigger and power-of-two pre-growth rule. Hopscotch finishes the fast-plan suite in about 126 s because the bitmap-guided neighbourhood keeps probe chains shallow. Robinhood completes the run in roughly 128 s, with the Robin Hood swaps containing probe growth after each resize. Cuckoo ends just under 128 s as the tighter load guard limits kick cascades across the two tables.

This shared heuristic highlights the trade-offs. Hopscotch delivers the best throughput when extra metadata is acceptable, Robinhood offers a compact option with predictable probe lengths, and Cuckoo supplies stable lookups once evenly balanced between tables. Every configuration passes the entire fast-plan suite, letting future workloads determine which map to deploy without compromising correctness.
 