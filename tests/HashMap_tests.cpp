#include <catch2/catch_test_macros.hpp>

#include "RHMap.h"

TEST_CASE("Initialiazation of Robin_Hood_HashMap", "[HashMap]") {
    RHMap<int, int> *map = new RHMap<int, int>(10ll);
    REQUIRE(map.get_capacity() == 10);
}