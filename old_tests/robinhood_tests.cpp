#include <catch2/catch_test_macros.hpp>
#include "RHMap.h"

//Test case to check if end() returns casted NULL pointer
TEST_CASE("End Function of RHMap", "[HashMap]") {
    RHMap<int, std::vector<int>> map(10ll);
    REQUIRE(map.end() == nullptr);
    RHMap<double, std::vector<int>> double_map(10ll);
    REQUIRE(double_map.end() == nullptr);
}

// Test case to test successful insertion and finding of keys
TEST_CASE("Insertion and Finding in RHMap", "[HashMap]") {
    RHMap<int, std::vector<int>> map(10ll);    
    map.emplace(1, std::vector<int>{2, 3});
    map.emplace(2, std::vector<int>{4, 5});
    map.emplace(3, std::vector<int>{6, 7});
    //Ensure find returns the right values for all inserted keys
    for (int i = 1; i <=3; ++i) {
        auto loc = map.find(i);
        REQUIRE(loc != map.end());
        REQUIRE(*loc == std::vector<int>{2*i, 2*i+1});
    }
    RHMap<double, std::vector<int>> double_map(10ll);
    double_map.emplace(1.5, std::vector<int>{52, 25});
    double_map.emplace(2.3, std::vector<int>{69, 96});
    double_map.emplace(3.14, std::vector<int>{67, 76});
    //Ensure find returns the right values for all inserted keys
    REQUIRE(*double_map.find(1.5) == std::vector<int>{52, 25});
    REQUIRE(*double_map.find(2.3) == std::vector<int>{69, 96});
    REQUIRE(*double_map.find(3.14) == std::vector<int>{67, 76});

}

// Test case to check if end() is returned when searching non-existent keys
TEST_CASE("Finding Non-Existent Keys in RHMap", "[HashMap]") {
    RHMap<int, std::vector<int>> map(10ll);
    map.emplace(1, std::vector<int>{2, 3});
    map.emplace(2, std::vector<int>{4, 5});
    map.emplace(3, std::vector<int>{6, 7});
    //Ensure find returns end() for non-existent keys
    for (int i = 4; i <=6; ++i) {
        auto loc = map.find(i);
        REQUIRE(loc == map.end());
    }
    RHMap<double, std::vector<int>> double_map(10ll);
    double_map.emplace(1.5, std::vector<int>{52, 25});
    double_map.emplace(2.3, std::vector<int>{69, 96});
    double_map.emplace(3.14, std::vector<int>{67, 76});
    //Ensure find returns end() for non-existent keys
    REQUIRE(double_map.find(4.2) == double_map.end());
    REQUIRE(double_map.find(5.5) == double_map.end());
    REQUIRE(double_map.find(6.6) == double_map.end());

}