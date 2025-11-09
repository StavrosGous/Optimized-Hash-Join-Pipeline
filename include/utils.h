#pragma once
#include <cstdint>
#include <cstring>
#include <iostream>
#include <zlib.h>
#ifndef CAPACITY
#define CAPACITY 64
#endif
#ifndef LOAD_FACTOR
#define LOAD_FACTOR 0.7
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



constexpr uint64_t offset_basis = 14695981039346656037ull;
constexpr uint64_t prime        = 1099511628211ull;

template <class T>
inline uint64_t fnv_hash(const T& v) {
    uint64_t h = offset_basis;

    if constexpr (std::is_same_v<T, std::string_view>) {
        const unsigned char* p = reinterpret_cast<const unsigned char*>(v.data());
        for (std::size_t i = 0; i < v.size(); ++i) {
            h ^= static_cast<uint64_t>(p[i]);
            h *= prime;
        }
        return h;
    } else if constexpr (std::is_same_v<T, int32_t>) {
        const unsigned char* p = reinterpret_cast<const unsigned char*>(&v);
        for (std::size_t i = 0; i < sizeof(v); ++i) {
            h ^= static_cast<uint64_t>(p[i]);
            h *= prime;
        }
        return h;
    } else if constexpr (std::is_same_v<T, int64_t>) {
        const unsigned char* p = reinterpret_cast<const unsigned char*>(&v);
        for (std::size_t i = 0; i < sizeof(v); ++i) {
            h ^= static_cast<uint64_t>(p[i]);
            h *= prime;
        }
        return h;
    } else if constexpr (std::is_same_v<T, double>) {
#if __cpp_lib_bit_cast >= 201806L
        uint64_t bits = std::bit_cast<uint64_t>(v);
#else
        static_assert(sizeof(double) == sizeof(uint64_t), "Unexpected double size");
        uint64_t bits;
        std::memcpy(&bits, &v, sizeof(bits));
#endif
        const unsigned char* p = reinterpret_cast<const unsigned char*>(&bits);
        for (std::size_t i = 0; i < sizeof(bits); ++i) {
            h ^= static_cast<uint64_t>(p[i]);
            h *= prime;
        }
        return h;
    } else {
        std::cout << "Unsupported key type for hashing" << std::endl;
		return 0ull;
    }
}



// ---------- SplitMix64 variant ----------
namespace splitmix {

// Core SplitMix64 mixer
inline uint64_t mix(uint64_t x) {
    x += 0x9E3779B97F4A7C15ull;
    x = (x ^ (x >> 30)) * 0xBF58476D1CE4E5B9ull;
    x = (x ^ (x >> 27)) * 0x94D049BB133111EBull;
    return x ^ (x >> 31);
}

// Normalize doubles: map both +0.0 and -0.0 to the same bit pattern.
// Remove this if you need to distinguish them.
inline uint64_t double_bits(double d) {
#if __cpp_lib_bit_cast >= 201806L
    uint64_t bits = std::bit_cast<uint64_t>(d);
#else
    static_assert(sizeof(double) == sizeof(uint64_t), "Unexpected double size");
    uint64_t bits; std::memcpy(&bits, &d, sizeof(bits));
#endif
    if ((bits << 1) == 0) bits = 0; // normalize signed zero
    return bits;
}

// Single entry point: supports int32_t, int64_t, double (exact match).
template <class T>
inline uint64_t hash(const T& v) {
    if constexpr (std::is_same_v<T, int32_t>) {
        return mix(static_cast<uint64_t>(static_cast<int64_t>(v)));
    } else if constexpr (std::is_same_v<T, int64_t>) {
        return mix(static_cast<uint64_t>(v));
    } else if constexpr (std::is_same_v<T, double>) {
        return mix(double_bits(v));
    } else {
        std::cout << "Unsupported key type for hashing" << std::endl;
        return 0ull;
    }
}

} // namespace splitmix_hash

// ---------- Wyhash final mixer variant ----------
namespace wyhash {

// Wyhash final 64-bit mixer (wyhash64/wyhash_final).
inline uint64_t mix(uint64_t x) {
    x ^= x >> 32;
    x *= 0xD6E8FEB86659FD93ull;
    x ^= x >> 32;
    x *= 0xD6E8FEB86659FD93ull;
    x ^= x >> 32;
    return x;
}

inline uint64_t double_bits(double d) {
#if __cpp_lib_bit_cast >= 201806L
    uint64_t bits = std::bit_cast<uint64_t>(d);
#else
    static_assert(sizeof(double) == sizeof(uint64_t), "Unexpected double size");
    uint64_t bits; std::memcpy(&bits, &d, sizeof(bits));
#endif
    if ((bits << 1) == 0) bits = 0; // normalize signed zero
    return bits;
}

// Single entry point: supports int32_t, int64_t, double (exact match).
template <class T>
inline uint64_t hash(const T& v) {
    if constexpr (std::is_same_v<T, int32_t>) {
        return mix(static_cast<uint64_t>(static_cast<int64_t>(v)));
    } else if constexpr (std::is_same_v<T, int64_t>) {
        return mix(static_cast<uint64_t>(v));
    } else if constexpr (std::is_same_v<T, double>) {
        return mix(double_bits(v));
    } else {
        std::cout << "Unsupported key type for hashing" << std::endl;
        return 0ull;
    }
}

} // namespace wyhash_hash