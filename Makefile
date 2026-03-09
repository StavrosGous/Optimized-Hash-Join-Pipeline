BUILD_DIR := build
JOBS := $(shell nproc)
CMAKE_FLAGS := -S . -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=Release -Wno-dev
DB_FILE := imdb.db
IMDB_DIR := imdb
CACHE_DIR := cache

.PHONY: all fast tests run run-tests clean configure setup download-imdb

all: fast

configure:
	cmake $(CMAKE_FLAGS)

fast: configure
	cmake --build $(BUILD_DIR) -- -j$(JOBS) fast

tests: configure
	cmake --build $(BUILD_DIR) -- -j$(JOBS) unit_tests

run: fast $(CACHE_DIR)
	./$(BUILD_DIR)/fast plans.json

run-tests: tests
	./$(BUILD_DIR)/unit_tests

clean:
	rm -rf $(BUILD_DIR)

# --- First-time setup ---

# Download IMDB dataset (only if imdb/ doesn't exist)
download-imdb: $(IMDB_DIR)

$(IMDB_DIR):
	./download_imdb.sh

# Build DuckDB database from CSV files (only if imdb.db doesn't exist)
$(DB_FILE): $(IMDB_DIR)
	cmake $(CMAKE_FLAGS)
	cmake --build $(BUILD_DIR) -- -j$(JOBS) build_database
	./$(BUILD_DIR)/build_database $(DB_FILE)

# Build the binary cache (only if cache/ doesn't exist)
$(CACHE_DIR): $(DB_FILE)
	cmake $(CMAKE_FLAGS)
	cmake --build $(BUILD_DIR) -- -j$(JOBS) build_cache
	./$(BUILD_DIR)/build_cache plans.json

# Full first-time setup: download data + build DB + build cache
setup: $(CACHE_DIR)
