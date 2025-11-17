#include <catch2/catch_test_macros.hpp>
#include "bitmap.h"
#include <iostream>
TEST_CASE("Check whether bitmap converts bits to bytes correctly", "[Bitmap]") {
    auto page_data = new std::byte[PAGE_SIZE];
    for (int i = 0; i < PAGE_SIZE; ++i) {
        page_data[i] = std::byte{0x55};
    }
    uint16_t num_rows = 32;
    auto bitmap = get_bitmap(page_data, num_rows);
    for (int i = 0; i < num_rows/8; ++i) {
        for (int j = 0; j < 8; ++j) {
            REQUIRE((bitmap[i] >> ((7-j) & 1)) == (j%2));
        }
    }
    if (num_rows % 8) {
        for (int i = 0; i < num_rows % 8; ++i) {
            REQUIRE((bitmap[num_rows/8] >> (7-i) & 1) == (i%2));
        }
    }
}
