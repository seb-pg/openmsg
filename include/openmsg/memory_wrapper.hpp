// openmsg by Sebastien Rubens
//
// To the extent possible under law, the person who associated CC0 with
// openmsg has waived all copyright and related or neighboring rights
// to openmsg.
//
// You should have received a copy of the CC0 legalcode along with this
// work.  If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.

#pragma once

#if (__cplusplus < 202002L) && !defined(_HAS_CXX20)
#error C++20 or more is needed
#endif

#include "openmsg/bswap.hpp"
#include "openmsg/type_traits.hpp"

#include <inttypes.h>
#include <bit>
#include <concepts>
#include <type_traits>

#if defined(__GNUC__) || defined(__clang__)
#include <byteswap.h>
#elif defined(_MSC_VER)
#include <stdlib.h>
#include "immintrin.h"
#endif

namespace openmsg {

// openmsg does not take side with regards to memory aliasing and
// memory access reordering issues (either software or hardware)

template<typename H, typename M, typename Endian>
struct memory_wrapper_bswap
{
    constexpr static auto endian = Endian::value;
    using U = as_uint_type_t<H>;

    constexpr static H mtoh(const M& x) noexcept
    {
        auto y = std::bit_cast<U>(x);
        if constexpr (endian != std::endian::native)
            y = bswap(y);
        return std::bit_cast<H>(y);
    }

    constexpr static M htom(const H& x) noexcept
    {
        auto y = std::bit_cast<U>(x);
        if constexpr (endian != std::endian::native)
            y = bswap(y);
        return std::bit_cast<M>(y);
    }
};

template<typename H, typename M, typename Endian>
struct memory_wrapper_robust
{
    constexpr static auto endian = Endian::value;
    using U = as_uint_type_t<H>;

    using bytes_t = uint8_t[sizeof(H)];
    constexpr static int mask = endian == std::endian::native ? 0 : (sizeof(H) - 1);

    constexpr static H mtoh(const M& x) noexcept
    {
        if (std::is_constant_evaluated())
            return memory_wrapper_bswap<H, M, Endian>::mtoh(x);
        auto y = std::bit_cast<U>(x);
        if constexpr (sizeof(M) == 1 || endian == std::endian::native)
            return std::bit_cast<H>(y);
        //
        U dst;
        auto values = reinterpret_cast<const bytes_t&>(y);
        dst = static_cast<U>(values[mask ^ 0]);
        if constexpr (sizeof(U) >= 2)
            dst = static_cast<U>(dst | (static_cast<U>(values[mask ^ 1]) << 8u));  // weird line to avoid: conversion from ‘int’ to ... {aka ‘short unsigned int’} may change value [-Warith-conversion]
        if constexpr (sizeof(U) >= 4)
            dst |= static_cast<U>(values[mask ^ 2]) << 16ul |
            static_cast<U>(values[mask ^ 3]) << 24ul;
        if constexpr (sizeof(U) >= 8)
            dst |= static_cast<U>(values[mask ^ 4]) << 32ull |
                   static_cast<U>(values[mask ^ 5]) << 40ull |
                   static_cast<U>(values[mask ^ 6]) << 48ull |
                   static_cast<U>(values[mask ^ 7]) << 56ull;
        return std::bit_cast<H>(dst);
    };

    constexpr static M htom(const H& x) noexcept
    {
        if (std::is_constant_evaluated())
            return memory_wrapper_bswap<H, M, Endian>::htom(x);
        auto y = std::bit_cast<U>(x);
        if constexpr (sizeof(M) == 1 || endian == std::endian::native)
            return std::bit_cast<M>(y);
        //
        M dst;
        auto values = reinterpret_cast<bytes_t&>(dst);
        values[mask ^ 0] = static_cast<uint8_t>(y);
        if constexpr (sizeof(U) >= 2)
            values[mask ^ 1] = static_cast<uint8_t>(y >> 8ul);
        if constexpr (sizeof(U) >= 4)
        {
            values[mask ^ 2] = static_cast<uint8_t>(y >> 16ul);
            values[mask ^ 3] = static_cast<uint8_t>(y >> 24ul);
        }
        if constexpr (sizeof(U) >= 8)
        {
            values[mask ^ 4] = static_cast<uint8_t>(y >> 32ul);
            values[mask ^ 5] = static_cast<uint8_t>(y >> 40ul);
            values[mask ^ 6] = static_cast<uint8_t>(y >> 48ul);
            values[mask ^ 7] = static_cast<uint8_t>(y >> 56ul);
        }
        return dst;
    };
};

template<typename H, typename M, typename Endian>
struct memory_wrapper_movbe
{
    // for Intel CPU of 4th generation Intel Core processor family (codenamed Haswell)
    constexpr static auto endian = Endian::value;
    using U = as_uint_type_t<H>;

    constexpr static H mtoh(const M& x) noexcept
    {
        if (std::is_constant_evaluated())
            return memory_wrapper_bswap<H, M, Endian>::mtoh(x);
        auto y = std::bit_cast<U>(x);
        if constexpr (sizeof(M) == 1 || endian == std::endian::native)
            return std::bit_cast<H>(y);
        //
#if defined(MSC_VER)
        if constexpr (sizeof(M) == 2)
            return std::bit_cast<H>(_load_be_u16(&y));
        if constexpr (sizeof(M) == 4)
            return std::bit_cast<H>(_load_be_u32(&y));
        if constexpr (sizeof(M) == 8)
            return std::bit_cast<H>(_load_be_u64(&y));
#else
        return memory_wrapper_bswap<H, M, Endian>::mtoh(x);
#endif
    }

    constexpr static M htom(const H& x) noexcept
    {
        if (std::is_constant_evaluated())
            return memory_wrapper_bswap<H, M, Endian>::htom(x);
        auto y = std::bit_cast<U>(x);
        if constexpr (sizeof(M) == 1 || endian == std::endian::native)
            return std::bit_cast<M>(y);
        //
#if defined(_MSC_VER)
        M dst;
        if constexpr (sizeof(M) == 2)
            _store_be_u16(&dst, y);
        if constexpr (sizeof(M) == 4)
            _store_be_u32(&dst, y);
        if constexpr (sizeof(M) == 8)
            _store_be_u64(&dst, y);
        return dst;
#else
        return memory_wrapper_bswap<H, M, Endian>::htom(x);
#endif
    }
};

}  // namespace openmsg
