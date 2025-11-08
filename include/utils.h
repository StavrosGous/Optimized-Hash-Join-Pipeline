#pragma once
#include <cstdint>
#include <cstring>
#include <iostream>
#include <zlib.h>
#ifndef CAPACITY
#define CAPACITY 32
#endif
#ifndef LOAD_FACTOR
#define LOAD_FACTOR 0.55
#endif





static inline uint64_t crc64(uint64_t key) {
	return __builtin_ia32_crc32di(key, 0);
}

static inline uint32_t crc32(uint32_t key) {
    return __builtin_ia32_crc32si(key, 0);
}

template<typename T>
uint64_t crc_hash(const T& key) {
	if constexpr (std::is_same_v<T, int32_t>) {
		return crc32(key) * ((0x8648DBDull << 32) + 1);
	} else if constexpr (std::is_same_v<T, int64_t>) {
		return crc64(key) * 0x2545F4914F6CDD1D;
	} else if constexpr (std::is_same_v<T, double>) {
		return crc64(*(uint64_t*)&key) * 0x2545F4914F6CDD1D;
	} else {
		std::cout << "Unsupported key type for hashing" << std::endl;
		return 0LL;
	}
}





inline std::uint64_t murmur_hash_64(const void* key, int len, std::uint64_t seed) {
	constexpr std::uint64_t m = 0xc6a4a7935bd1e995ULL;
	constexpr int r = 47;
	std::uint64_t h = seed ^ (static_cast<std::uint64_t>(len) * m);
	const auto* data = static_cast<const unsigned char*>(key);
	const auto* end = data + (len & ~7);
	while (data != end) {
		std::uint64_t k;
		std::memcpy(&k, data, sizeof(std::uint64_t));
		data += sizeof(std::uint64_t);
#if defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
		k = __builtin_bswap64(k);
#endif
		k *= m;
		k ^= k >> r;
		k *= m;
		h ^= k;
		h *= m;
	}
	const auto* tail = data;
	switch (len & 7) {
		case 7: h ^= static_cast<std::uint64_t>(tail[6]) << 48; [[fallthrough]];
		case 6: h ^= static_cast<std::uint64_t>(tail[5]) << 40; [[fallthrough]];
		case 5: h ^= static_cast<std::uint64_t>(tail[4]) << 32; [[fallthrough]];
		case 4: h ^= static_cast<std::uint64_t>(tail[3]) << 24; [[fallthrough]];
		case 3: h ^= static_cast<std::uint64_t>(tail[2]) << 16; [[fallthrough]];
		case 2: h ^= static_cast<std::uint64_t>(tail[1]) << 8; [[fallthrough]];
		case 1: h ^= static_cast<std::uint64_t>(tail[0]); h *= m;
	}
	h ^= h >> r;
	h *= m;
	h ^= h >> r;
	return h;
}

