#include <catch2/catch_test_macros.hpp>

#include "RHMap.h"

// Test case to verify initialization of Robin_Hood_HashMap
TEST_CASE("Initialization of Robin_Hood_HashMap", "[HashMap]") {
    RHMap<int, std::vector<int>> *map = new RHMap<int, std::vector<int>>(10ll);
    map->emplace(1, std::vector<int>{2, 3});
    map->emplace(2, std::vector<int>{4, 5});
    map->emplace(3, std::vector<int>{6, 7});
    REQUIRE(map->get_count() == 3);
    REQUIRE(map->get_capacity() == 1 << (sizeof(size_t) * 8 - __builtin_clzll(10 - 1)));
    delete map;
}
//Test case to check if end() returns casted NULL pointer
TEST_CASE("End Function of Robin_Hood_HashMap", "[HashMap]") {
    RHMap<int, std::vector<int>> map(10ll);
    auto end_ptr = map.end();
    REQUIRE(end_ptr == nullptr);
}

// Test case to test successful insertion and finding of keys
TEST_CASE("Insertion and Finding in Robin_Hood_HashMap", "[HashMap]") {
    RHMap<int, std::vector<int>> map(10ll);
    map.emplace(1, std::vector<int>{2, 3});
    map.emplace(2, std::vector<int>{4, 5});
    map.emplace(3, std::vector<int>{6, 7});
    //Ensure find returns the right values for all inserted keys
    for (int i = 1; i <=3; ++i) {
        auto &itr = map.find(i);
        REQUIRE(itr != map.end());
        REQUIRE(itr->second == std::vector<int>{2*i, 2*i+1});
    }
}

// Test case to check if end() is returned when searching non-existent keys
TEST_CASE("Finding Non-Existent Keys in Robin_Hood_HashMap", "[HashMap]") {
    RHMap<int, std::vector<int>> map(10ll);
    map.emplace(1, std::vector<int>{2, 3});
    map.emplace(2, std::vector<int>{4, 5});
    map.emplace(3, std::vector<int>{6, 7});
    //Ensure find returns end() for non-existent keys
    for (int i = 4; i <=6; ++i) {
        auto &itr = map.find(i);
        REQUIRE(itr == map.end());
    }
}