#include <catch2/catch_test_macros.hpp>
#include "CuckooMap.h"

//Test case to check if end() returns casted NULL pointer
TEST_CASE("End Function of CuckooMap", "[HashMap]") {
    CuckooMap<int, std::vector<int>> map(10ll);
    auto end_ptr = map.end();
    REQUIRE(end_ptr == nullptr);
}

// Test case to test successful insertion and finding of keys
TEST_CASE("Insertion and Finding in CuckooMap", "[HashMap]") {
    CuckooMap<int, std::vector<int>> map(10ll);
    map.emplace(1, std::vector<int>{2, 3});
    map.emplace(2, std::vector<int>{4, 5});
    map.emplace(3, std::vector<int>{6, 7});
    //Ensure find returns the right values for all inserted keys
    for (int i = 1; i <=3; ++i) {
        auto loc = map.find(i);
        REQUIRE(loc != map.end());
        REQUIRE(*loc == std::vector<int>{2*i, 2*i+1});
    }
}

// Test case to check if end() is returned when searching non-existent keys
TEST_CASE("Finding Non-Existent Keys in CuckooMap", "[HashMap]") {
    CuckooMap<int, std::vector<int>> map(10ll);
    map.emplace(1, std::vector<int>{2, 3});
    map.emplace(2, std::vector<int>{4, 5});
    map.emplace(3, std::vector<int>{6, 7});
    //Ensure find returns end() for non-existent keys
    for (int i = 4; i <=6; ++i) {
        auto loc = map.find(i);
        REQUIRE(loc == map.end());
    }
}