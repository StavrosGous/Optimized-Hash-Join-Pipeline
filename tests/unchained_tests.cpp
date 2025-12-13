#include <catch2/catch_test_macros.hpp>
#include "unchained_ht.h"

namespace {

// Helper function to create int32 column to use for hashmap
column_t make_int32_column(const std::vector<int32_t>& values) {
    column_t col(DataType::INT32);
    buffer_t buf;
    for (size_t i = 0; i < values.size(); ++i) {
        if (buf.count == MAX_PER_BUFFER_ENTRY) {
            col.buffers.push_back(std::move(buf));
            buf = buffer_t{};
        }
        value_t v;
        v.int32_val.val = values[i];
        v.int32_val.status = 1;
        buf.data[buf.count++] = v;
        ++col.num_rows;
    }
    if (buf.count > 0) {
        col.buffers.push_back(std::move(buf));
    }

    return col;
}

// Also Helper function that creates a column for the hashtable, but with NULL values too
column_t make_int32_column_with_nulls(const std::vector<std::pair<int32_t, bool>>& values) {
    column_t col(DataType::INT32);
    buffer_t buf;
    for (size_t i = 0; i < values.size(); ++i) {
        if (buf.count == MAX_PER_BUFFER_ENTRY) {
            col.buffers.push_back(std::move(buf));
            buf = buffer_t{};
        }
        value_t val;
        val.int32_val.val = values[i].first;
        val.int32_val.status = values[i].second ? 1 : 0;
        buf.data[buf.count++] = val;
        ++col.num_rows;
    }
    if (buf.count > 0) {
        col.buffers.push_back(std::move(buf));
    }

    return col;
}

}

TEST_CASE("UnchainedHashTable constructs and returns empty range on empty build", "[UnchainedHashTable]") {
    column_t empty_col(DataType::INT32);
    UnchainedHashTable ht(1);
    ht.build(empty_col);
    auto [start, end] = ht.lookup(42);
    REQUIRE(start == nullptr);
    REQUIRE(end == nullptr);
}

TEST_CASE("UnchainedHashTable build+lookup returns a valid range for existing key", "[UnchainedHashTable]") {
    auto col = make_int32_column({10, 20, 30, 40, 50});
    UnchainedHashTable ht(col.num_rows);
    ht.build(col);
    auto [start, end] = ht.lookup(30);
    REQUIRE(start != nullptr);
    REQUIRE(end != nullptr);
    REQUIRE(start <= end);
    // Ensure the key exists inside the returned bucket range
    bool found = false;
    for (auto cur = start; cur < end; ++cur) {
        if (cur->key == 30) {
            found = true;
            break;
        }
    }
    REQUIRE(found);
}

TEST_CASE("UnchainedHashTable lookup returns empty range for non-existent keys", "[UnchainedHashTable]") {
    auto col = make_int32_column({1, 2, 3, 4, 5, 6});
    UnchainedHashTable ht(col.num_rows);
    ht.build(col);
    for (int32_t k : {0, 7, 42, -1}) {
        auto [start, end] = ht.lookup(k);
        REQUIRE(start == nullptr);
        REQUIRE(end == nullptr);
    }
}

TEST_CASE("UnchainedHashTable ignores NULL (status=0) values when being built", "[UnchainedHashTable]") {
    auto col = make_int32_column_with_nulls({
        {9, true},
        {21, true},
        {33, false}, // NULL
        {45, true},
    });
    UnchainedHashTable ht(col.num_rows);
    ht.build(col);
    // 33 was NULL, so lookup must fail
    {
        auto [start, end] = ht.lookup(33);
        REQUIRE(start == nullptr);
        REQUIRE(end == nullptr);
    }
    // Non-null existing key should still be found
    bool found = false;
    {
        auto [start, end] = ht.lookup(21);
        REQUIRE(start != nullptr);
        REQUIRE(end != nullptr);
        for (auto cur = start; cur < end; ++cur) {
            if (cur->key == 21) {
                found = true;
                break;
            }
        }
    }
    REQUIRE(found);
}

TEST_CASE("UnchainedHashTable supports duplicates", "[UnchainedHashTable]") {
    auto col = make_int32_column({5, 5, 5, 7, 8, 5});
    UnchainedHashTable ht(col.num_rows);
    ht.build(col);
    auto [start, end] = ht.lookup(5);
    REQUIRE(start != nullptr);
    REQUIRE(end != nullptr);
    size_t matches = 0;
    for (auto cur = start; cur < end; ++cur) {
        if (cur->key == 5) {
            ++matches;
        }
    }
    REQUIRE(matches == 4);
}


