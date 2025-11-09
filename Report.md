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
Runtime measurements captured from `./run.sh fast_plans` which includes test queries `1a-33c` had a mean total runtime of 133.2 seconds.
In detail one indicative run had the following durations:
```console
>$ ./build.sh fast_plans 0
...
>$ ./run.sh fast_plans
Query 1a >>              Runtime: 364 ms - Result correct: true
Query 1b >>              Runtime: 245 ms - Result correct: true
Query 1c >>              Runtime: 126 ms - Result correct: true
Query 1d >>              Runtime: 279 ms - Result correct: true
Query 2a >>              Runtime: 626 ms - Result correct: true
Query 2b >>              Runtime: 557 ms - Result correct: true
Query 2c >>              Runtime: 530 ms - Result correct: true
Query 2d >>              Runtime: 706 ms - Result correct: true
Query 3a >>              Runtime: 325 ms - Result correct: true
Query 3b >>              Runtime: 267 ms - Result correct: true
Query 3c >>              Runtime: 386 ms - Result correct: true
Query 4a >>              Runtime: 351 ms - Result correct: true
Query 4b >>              Runtime: 275 ms - Result correct: true
Query 4c >>              Runtime: 426 ms - Result correct: true
Query 5a >>              Runtime: 79 ms - Result correct: true
Query 5b >>              Runtime: 52 ms - Result correct: true
Query 5c >>              Runtime: 172 ms - Result correct: true
Query 6a >>              Runtime: 2435 ms - Result correct: true
Query 6b >>              Runtime: 2105 ms - Result correct: true
Query 6c >>              Runtime: 2411 ms - Result correct: true
Query 6d >>              Runtime: 2180 ms - Result correct: true
Query 6e >>              Runtime: 2556 ms - Result correct: true
Query 6f >>              Runtime: 3264 ms - Result correct: true
Query 7a >>              Runtime: 4196 ms - Result correct: true
Query 7b >>              Runtime: 1939 ms - Result correct: true
Query 7c >>              Runtime: 2533 ms - Result correct: true
Query 8a >>              Runtime: 252 ms - Result correct: true
Query 8b >>              Runtime: 69 ms - Result correct: true
Query 8c >>              Runtime: 5339 ms - Result correct: true
Query 8d >>              Runtime: 4341 ms - Result correct: true
Query 9a >>              Runtime: 458 ms - Result correct: true
Query 9b >>              Runtime: 393 ms - Result correct: true
Query 9c >>              Runtime: 1329 ms - Result correct: true
Query 9d >>              Runtime: 1952 ms - Result correct: true
Query 10a >>             Runtime: 446 ms - Result correct: true
Query 10b >>             Runtime: 1169 ms - Result correct: true
Query 10c >>             Runtime: 1815 ms - Result correct: true
Query 11a >>             Runtime: 427 ms - Result correct: true
Query 11b >>             Runtime: 296 ms - Result correct: true
Query 11c >>             Runtime: 492 ms - Result correct: true
Query 11d >>             Runtime: 536 ms - Result correct: true
Query 12a >>             Runtime: 212 ms - Result correct: true
Query 12b >>             Runtime: 1454 ms - Result correct: true
Query 12c >>             Runtime: 294 ms - Result correct: true
Query 13a >>             Runtime: 2304 ms - Result correct: true
Query 13b >>             Runtime: 1043 ms - Result correct: true
Query 13c >>             Runtime: 1082 ms - Result correct: true
Query 13d >>             Runtime: 3282 ms - Result correct: true
Query 14a >>             Runtime: 441 ms - Result correct: true
Query 14b >>             Runtime: 307 ms - Result correct: true
Query 14c >>             Runtime: 461 ms - Result correct: true
Query 15a >>             Runtime: 329 ms - Result correct: true
Query 15b >>             Runtime: 275 ms - Result correct: true
Query 15c >>             Runtime: 500 ms - Result correct: true
Query 15d >>             Runtime: 506 ms - Result correct: true
Query 16a >>             Runtime: 3374 ms - Result correct: true
Query 16b >>             Runtime: 4907 ms - Result correct: true
Query 16c >>             Runtime: 2504 ms - Result correct: true
Query 16d >>             Runtime: 2484 ms - Result correct: true
Query 17a >>             Runtime: 2778 ms - Result correct: true
Query 17b >>             Runtime: 2145 ms - Result correct: true
Query 17c >>             Runtime: 2072 ms - Result correct: true
Query 17d >>             Runtime: 2114 ms - Result correct: true
Query 17e >>             Runtime: 3261 ms - Result correct: true
Query 17f >>             Runtime: 2826 ms - Result correct: true
Query 18a >>             Runtime: 1559 ms - Result correct: true
Query 18b >>             Runtime: 163 ms - Result correct: true
Query 18c >>             Runtime: 503 ms - Result correct: true
Query 19a >>             Runtime: 306 ms - Result correct: true
Query 19b >>             Runtime: 212 ms - Result correct: true
Query 19c >>             Runtime: 470 ms - Result correct: true
Query 19d >>             Runtime: 3071 ms - Result correct: true
Query 20a >>             Runtime: 2629 ms - Result correct: true
Query 20b >>             Runtime: 2403 ms - Result correct: true
Query 20c >>             Runtime: 2846 ms - Result correct: true
Query 21a >>             Runtime: 433 ms - Result correct: true
Query 21b >>             Runtime: 420 ms - Result correct: true
Query 21c >>             Runtime: 519 ms - Result correct: true
Query 22a >>             Runtime: 472 ms - Result correct: true
Query 22b >>             Runtime: 455 ms - Result correct: true
Query 22c >>             Runtime: 515 ms - Result correct: true
Query 22d >>             Runtime: 655 ms - Result correct: true
Query 23a >>             Runtime: 443 ms - Result correct: true
Query 23b >>             Runtime: 510 ms - Result correct: true
Query 23c >>             Runtime: 476 ms - Result correct: true
Query 24a >>             Runtime: 674 ms - Result correct: true
Query 24b >>             Runtime: 655 ms - Result correct: true
Query 25a >>             Runtime: 638 ms - Result correct: true
Query 25b >>             Runtime: 475 ms - Result correct: true
Query 25c >>             Runtime: 717 ms - Result correct: true
Query 26a >>             Runtime: 3168 ms - Result correct: true
Query 26b >>             Runtime: 2611 ms - Result correct: true
Query 26c >>             Runtime: 3571 ms - Result correct: true
Query 27a >>             Runtime: 385 ms - Result correct: true
Query 27b >>             Runtime: 343 ms - Result correct: true
Query 27c >>             Runtime: 476 ms - Result correct: true
Query 28a >>             Runtime: 546 ms - Result correct: true
Query 28b >>             Runtime: 355 ms - Result correct: true
Query 28c >>             Runtime: 456 ms - Result correct: true
Query 29a >>             Runtime: 615 ms - Result correct: true
Query 29b >>             Runtime: 593 ms - Result correct: true
Query 29c >>             Runtime: 981 ms - Result correct: true
Query 30a >>             Runtime: 578 ms - Result correct: true
Query 30b >>             Runtime: 498 ms - Result correct: true
Query 30c >>             Runtime: 731 ms - Result correct: true
Query 31a >>             Runtime: 809 ms - Result correct: true
Query 31b >>             Runtime: 487 ms - Result correct: true
Query 31c >>             Runtime: 959 ms - Result correct: true
Query 32a >>             Runtime: 557 ms - Result correct: true
Query 32b >>             Runtime: 581 ms - Result correct: true
Query 33a >>             Runtime: 619 ms - Result correct: true
Query 33b >>             Runtime: 552 ms - Result correct: true
Query 33c >>             Runtime: 818 ms - Result correct: true
Total runtime: 133162 ms
```

## Fast with Hopscotch

Runtime measurements captured from `./run.sh fast_plans` which includes test queries `1a-33c` had a mean total runtime of 130.8 seconds.
In detail one indicative run had the following durations:
```console
>$ ./build.sh fast_plans 1
...
>$ ./run.sh fast_plans
Query 1a >>              Runtime: 363 ms - Result correct: true
Query 1b >>              Runtime: 243 ms - Result correct: true
Query 1c >>              Runtime: 124 ms - Result correct: true
Query 1d >>              Runtime: 281 ms - Result correct: true
Query 2a >>              Runtime: 630 ms - Result correct: true
Query 2b >>              Runtime: 603 ms - Result correct: true
Query 2c >>              Runtime: 569 ms - Result correct: true
Query 2d >>              Runtime: 721 ms - Result correct: true
Query 3a >>              Runtime: 309 ms - Result correct: true
Query 3b >>              Runtime: 271 ms - Result correct: true
Query 3c >>              Runtime: 380 ms - Result correct: true
Query 4a >>              Runtime: 345 ms - Result correct: true
Query 4b >>              Runtime: 257 ms - Result correct: true
Query 4c >>              Runtime: 414 ms - Result correct: true
Query 5a >>              Runtime: 75 ms - Result correct: true
Query 5b >>              Runtime: 51 ms - Result correct: true
Query 5c >>              Runtime: 165 ms - Result correct: true
Query 6a >>              Runtime: 2408 ms - Result correct: true
Query 6b >>              Runtime: 2057 ms - Result correct: true
Query 6c >>              Runtime: 2385 ms - Result correct: true
Query 6d >>              Runtime: 2206 ms - Result correct: true
Query 6e >>              Runtime: 2515 ms - Result correct: true
Query 6f >>              Runtime: 3038 ms - Result correct: true
Query 7a >>              Runtime: 3687 ms - Result correct: true
Query 7b >>              Runtime: 1836 ms - Result correct: true
Query 7c >>              Runtime: 2606 ms - Result correct: true
Query 8a >>              Runtime: 271 ms - Result correct: true
Query 8b >>              Runtime: 81 ms - Result correct: true
Query 8c >>              Runtime: 4600 ms - Result correct: true
Query 8d >>              Runtime: 3503 ms - Result correct: true
Query 9a >>              Runtime: 466 ms - Result correct: true
Query 9b >>              Runtime: 388 ms - Result correct: true
Query 9c >>              Runtime: 924 ms - Result correct: true
Query 9d >>              Runtime: 1434 ms - Result correct: true
Query 10a >>             Runtime: 443 ms - Result correct: true
Query 10b >>             Runtime: 704 ms - Result correct: true
Query 10c >>             Runtime: 1185 ms - Result correct: true
Query 11a >>             Runtime: 398 ms - Result correct: true
Query 11b >>             Runtime: 279 ms - Result correct: true
Query 11c >>             Runtime: 509 ms - Result correct: true
Query 11d >>             Runtime: 556 ms - Result correct: true
Query 12a >>             Runtime: 220 ms - Result correct: true
Query 12b >>             Runtime: 1503 ms - Result correct: true
Query 12c >>             Runtime: 334 ms - Result correct: true
Query 13a >>             Runtime: 2079 ms - Result correct: true
Query 13b >>             Runtime: 1030 ms - Result correct: true
Query 13c >>             Runtime: 1056 ms - Result correct: true
Query 13d >>             Runtime: 3230 ms - Result correct: true
Query 14a >>             Runtime: 407 ms - Result correct: true
Query 14b >>             Runtime: 271 ms - Result correct: true
Query 14c >>             Runtime: 447 ms - Result correct: true
Query 15a >>             Runtime: 323 ms - Result correct: true
Query 15b >>             Runtime: 275 ms - Result correct: true
Query 15c >>             Runtime: 490 ms - Result correct: true
Query 15d >>             Runtime: 506 ms - Result correct: true
Query 16a >>             Runtime: 3286 ms - Result correct: true
Query 16b >>             Runtime: 4640 ms - Result correct: true
Query 16c >>             Runtime: 2505 ms - Result correct: true
Query 16d >>             Runtime: 2411 ms - Result correct: true
Query 17a >>             Runtime: 2539 ms - Result correct: true
Query 17b >>             Runtime: 2210 ms - Result correct: true
Query 17c >>             Runtime: 2115 ms - Result correct: true
Query 17d >>             Runtime: 2124 ms - Result correct: true
Query 17e >>             Runtime: 3020 ms - Result correct: true
Query 17f >>             Runtime: 2838 ms - Result correct: true
Query 18a >>             Runtime: 1506 ms - Result correct: true
Query 18b >>             Runtime: 155 ms - Result correct: true
Query 18c >>             Runtime: 540 ms - Result correct: true
Query 19a >>             Runtime: 294 ms - Result correct: true
Query 19b >>             Runtime: 203 ms - Result correct: true
Query 19c >>             Runtime: 479 ms - Result correct: true
Query 19d >>             Runtime: 2638 ms - Result correct: true
Query 20a >>             Runtime: 2735 ms - Result correct: true
Query 20b >>             Runtime: 3154 ms - Result correct: true
Query 20c >>             Runtime: 3603 ms - Result correct: true
Query 21a >>             Runtime: 397 ms - Result correct: true
Query 21b >>             Runtime: 394 ms - Result correct: true
Query 21c >>             Runtime: 488 ms - Result correct: true
Query 22a >>             Runtime: 453 ms - Result correct: true
Query 22b >>             Runtime: 530 ms - Result correct: true
Query 22c >>             Runtime: 513 ms - Result correct: true
Query 22d >>             Runtime: 667 ms - Result correct: true
Query 23a >>             Runtime: 490 ms - Result correct: true
Query 23b >>             Runtime: 460 ms - Result correct: true
Query 23c >>             Runtime: 485 ms - Result correct: true
Query 24a >>             Runtime: 770 ms - Result correct: true
Query 24b >>             Runtime: 645 ms - Result correct: true
Query 25a >>             Runtime: 667 ms - Result correct: true
Query 25b >>             Runtime: 504 ms - Result correct: true
Query 25c >>             Runtime: 715 ms - Result correct: true
Query 26a >>             Runtime: 3874 ms - Result correct: true
Query 26b >>             Runtime: 2770 ms - Result correct: true
Query 26c >>             Runtime: 3357 ms - Result correct: true
Query 27a >>             Runtime: 469 ms - Result correct: true
Query 27b >>             Runtime: 383 ms - Result correct: true
Query 27c >>             Runtime: 531 ms - Result correct: true
Query 28a >>             Runtime: 600 ms - Result correct: true
Query 28b >>             Runtime: 440 ms - Result correct: true
Query 28c >>             Runtime: 554 ms - Result correct: true
Query 29a >>             Runtime: 722 ms - Result correct: true
Query 29b >>             Runtime: 715 ms - Result correct: true
Query 29c >>             Runtime: 1038 ms - Result correct: true
Query 30a >>             Runtime: 727 ms - Result correct: true
Query 30b >>             Runtime: 610 ms - Result correct: true
Query 30c >>             Runtime: 852 ms - Result correct: true
Query 31a >>             Runtime: 826 ms - Result correct: true
Query 31b >>             Runtime: 489 ms - Result correct: true
Query 31c >>             Runtime: 986 ms - Result correct: true
Query 32a >>             Runtime: 540 ms - Result correct: true
Query 32b >>             Runtime: 602 ms - Result correct: true
Query 33a >>             Runtime: 604 ms - Result correct: true
Query 33b >>             Runtime: 648 ms - Result correct: true
Query 33c >>             Runtime: 799 ms - Result correct: true
Total runtime: 130760 ms
```

## Fast with Cuckoo 

Runtime measurements captured from `./run.sh fast_plans` which includes test queries `1a-33c` had a mean total runtime of 136.6 seconds.
In detail one indicative run had the following durations:
```console
>$ ./build.sh fast_plans 2
...
>$ ./run.sh fast_plans
Query 1a >>              Runtime: 356 ms - Result correct: true
Query 1b >>              Runtime: 250 ms - Result correct: true
Query 1c >>              Runtime: 123 ms - Result correct: true
Query 1d >>              Runtime: 283 ms - Result correct: true
Query 2a >>              Runtime: 594 ms - Result correct: true
Query 2b >>              Runtime: 523 ms - Result correct: true
Query 2c >>              Runtime: 514 ms - Result correct: true
Query 2d >>              Runtime: 698 ms - Result correct: true
Query 3a >>              Runtime: 297 ms - Result correct: true
Query 3b >>              Runtime: 256 ms - Result correct: true
Query 3c >>              Runtime: 367 ms - Result correct: true
Query 4a >>              Runtime: 352 ms - Result correct: true
Query 4b >>              Runtime: 252 ms - Result correct: true
Query 4c >>              Runtime: 439 ms - Result correct: true
Query 5a >>              Runtime: 66 ms - Result correct: true
Query 5b >>              Runtime: 50 ms - Result correct: true
Query 5c >>              Runtime: 168 ms - Result correct: true
Query 6a >>              Runtime: 2438 ms - Result correct: true
Query 6b >>              Runtime: 2046 ms - Result correct: true
Query 6c >>              Runtime: 2417 ms - Result correct: true
Query 6d >>              Runtime: 2564 ms - Result correct: true
Query 6e >>              Runtime: 2478 ms - Result correct: true
Query 6f >>              Runtime: 3596 ms - Result correct: true
Query 7a >>              Runtime: 4485 ms - Result correct: true
Query 7b >>              Runtime: 1978 ms - Result correct: true
Query 7c >>              Runtime: 2628 ms - Result correct: true
Query 8a >>              Runtime: 263 ms - Result correct: true
Query 8b >>              Runtime: 79 ms - Result correct: true
Query 8c >>              Runtime: 4959 ms - Result correct: true
Query 8d >>              Runtime: 3885 ms - Result correct: true
Query 9a >>              Runtime: 433 ms - Result correct: true
Query 9b >>              Runtime: 347 ms - Result correct: true
Query 9c >>              Runtime: 1393 ms - Result correct: true
Query 9d >>              Runtime: 2107 ms - Result correct: true
Query 10a >>             Runtime: 442 ms - Result correct: true
Query 10b >>             Runtime: 1237 ms - Result correct: true
Query 10c >>             Runtime: 1850 ms - Result correct: true
Query 11a >>             Runtime: 405 ms - Result correct: true
Query 11b >>             Runtime: 291 ms - Result correct: true
Query 11c >>             Runtime: 498 ms - Result correct: true
Query 11d >>             Runtime: 529 ms - Result correct: true
Query 12a >>             Runtime: 203 ms - Result correct: true
Query 12b >>             Runtime: 1542 ms - Result correct: true
Query 12c >>             Runtime: 289 ms - Result correct: true
Query 13a >>             Runtime: 2396 ms - Result correct: true
Query 13b >>             Runtime: 1216 ms - Result correct: true
Query 13c >>             Runtime: 1197 ms - Result correct: true
Query 13d >>             Runtime: 3534 ms - Result correct: true
Query 14a >>             Runtime: 405 ms - Result correct: true
Query 14b >>             Runtime: 310 ms - Result correct: true
Query 14c >>             Runtime: 457 ms - Result correct: true
Query 15a >>             Runtime: 326 ms - Result correct: true
Query 15b >>             Runtime: 285 ms - Result correct: true
Query 15c >>             Runtime: 526 ms - Result correct: true
Query 15d >>             Runtime: 502 ms - Result correct: true
Query 16a >>             Runtime: 3641 ms - Result correct: true
Query 16b >>             Runtime: 5567 ms - Result correct: true
Query 16c >>             Runtime: 2657 ms - Result correct: true
Query 16d >>             Runtime: 2599 ms - Result correct: true
Query 17a >>             Runtime: 2816 ms - Result correct: true
Query 17b >>             Runtime: 2185 ms - Result correct: true
Query 17c >>             Runtime: 2089 ms - Result correct: true
Query 17d >>             Runtime: 2112 ms - Result correct: true
Query 17e >>             Runtime: 3653 ms - Result correct: true
Query 17f >>             Runtime: 3005 ms - Result correct: true
Query 18a >>             Runtime: 1498 ms - Result correct: true
Query 18b >>             Runtime: 155 ms - Result correct: true
Query 18c >>             Runtime: 563 ms - Result correct: true
Query 19a >>             Runtime: 302 ms - Result correct: true
Query 19b >>             Runtime: 213 ms - Result correct: true
Query 19c >>             Runtime: 454 ms - Result correct: true
Query 19d >>             Runtime: 3299 ms - Result correct: true
Query 20a >>             Runtime: 2562 ms - Result correct: true
Query 20b >>             Runtime: 2300 ms - Result correct: true
Query 20c >>             Runtime: 2839 ms - Result correct: true
Query 21a >>             Runtime: 467 ms - Result correct: true
Query 21b >>             Runtime: 426 ms - Result correct: true
Query 21c >>             Runtime: 545 ms - Result correct: true
Query 22a >>             Runtime: 469 ms - Result correct: true
Query 22b >>             Runtime: 459 ms - Result correct: true
Query 22c >>             Runtime: 514 ms - Result correct: true
Query 22d >>             Runtime: 682 ms - Result correct: true
Query 23a >>             Runtime: 480 ms - Result correct: true
Query 23b >>             Runtime: 480 ms - Result correct: true
Query 23c >>             Runtime: 466 ms - Result correct: true
Query 24a >>             Runtime: 680 ms - Result correct: true
Query 24b >>             Runtime: 639 ms - Result correct: true
Query 25a >>             Runtime: 633 ms - Result correct: true
Query 25b >>             Runtime: 466 ms - Result correct: true
Query 25c >>             Runtime: 733 ms - Result correct: true
Query 26a >>             Runtime: 3251 ms - Result correct: true
Query 26b >>             Runtime: 2815 ms - Result correct: true
Query 26c >>             Runtime: 3440 ms - Result correct: true
Query 27a >>             Runtime: 382 ms - Result correct: true
Query 27b >>             Runtime: 330 ms - Result correct: true
Query 27c >>             Runtime: 493 ms - Result correct: true
Query 28a >>             Runtime: 567 ms - Result correct: true
Query 28b >>             Runtime: 365 ms - Result correct: true
Query 28c >>             Runtime: 475 ms - Result correct: true
Query 29a >>             Runtime: 657 ms - Result correct: true
Query 29b >>             Runtime: 626 ms - Result correct: true
Query 29c >>             Runtime: 1023 ms - Result correct: true
Query 30a >>             Runtime: 615 ms - Result correct: true
Query 30b >>             Runtime: 515 ms - Result correct: true
Query 30c >>             Runtime: 766 ms - Result correct: true
Query 31a >>             Runtime: 816 ms - Result correct: true
Query 31b >>             Runtime: 512 ms - Result correct: true
Query 31c >>             Runtime: 961 ms - Result correct: true
Query 32a >>             Runtime: 605 ms - Result correct: true
Query 32b >>             Runtime: 608 ms - Result correct: true
Query 33a >>             Runtime: 616 ms - Result correct: true
Query 33b >>             Runtime: 593 ms - Result correct: true
Query 33c >>             Runtime: 858 ms - Result correct: true
Total runtime: 136661 ms
```