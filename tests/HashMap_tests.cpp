#include <catch2/catch_test_macros.hpp>

#include "RHMap.h"

TEST_CASE("Initialization of Robin_Hood_HashMap", "[HashMap]") {
    RHMap<int, std::vector<int>> *map = new RHMap<int, std::vector<int>>(10ll);
    map->emplace(1, std::vector<int>{2, 3});
    map->emplace(2, std::vector<int>{4, 5});
    map->emplace(3, std::vector<int>{6, 7});
    REQUIRE(map->get_count() == 3);
    REQUIRE(map->get_capacity() == 1 << (sizeof(size_t) * 8 - __builtin_clzll(10 - 1)));
    delete map;
}