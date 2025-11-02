#pragma once

#include <cstdint>
#include <cstring>

inline std::uint64_t splitmix64(std::uint64_t x) {
	x += 0x9e3779b97f4a7c15ULL;
	x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9ULL;
	x = (x ^ (x >> 27)) * 0x94d049bb133111ebULL;
	return x ^ (x >> 31);
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

