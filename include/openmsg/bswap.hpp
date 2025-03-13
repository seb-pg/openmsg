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

#include "openmsg/concepts.hpp"
#include "openmsg/type_traits.hpp"

#include <inttypes.h>
#include <bit>
#include <concepts>
#include <type_traits>

#if defined(__GNUC__) || defined(__clang__)
#include <byteswap.h>
#elif defined(_MSC_VER)
#include <stdlib.h>
#endif

namespace openmsg {

namespace detail_constexpr {

template<std::integral T>
constexpr T __bswap(T value) noexcept
{
    if constexpr (sizeof(T) == 1)
        return value;
    using U = as_uint_type_t<T>;
    using H = as_half_size_t<T>;
    constexpr U shift = sizeof(T) * 4;
    U unsigned_value = static_cast<U>(value);
    auto msw = __bswap(static_cast<H>(unsigned_value));
    auto lsw = __bswap(static_cast<H>(unsigned_value >> shift));
    return static_cast<T>(static_cast<U>(msw) << shift | static_cast<U>(lsw));
}

template<swappable T>
constexpr T bswap(T value) noexcept
{
    using U = as_uint_type_t<T>;
    return std::bit_cast<T>(__bswap(std::bit_cast<U>(value)));
}

}  // namespace detail_constexpr

template<swappable T>
constexpr T bswap(T x) noexcept
{
    if (std::is_constant_evaluated())
        return detail_constexpr::bswap(x);
    if constexpr (sizeof(T) == 1)
        return x;
    using U = as_uint_type_t<T>;
#if defined(__GNUC__) || defined(__clang__)
    if constexpr (sizeof(T) == 2)
        return std::bit_cast<T>(bswap_16(std::bit_cast<U>(x)));
    if constexpr (sizeof(T) == 4)
        return std::bit_cast<T>(bswap_32(std::bit_cast<U>(x)));
    if constexpr (sizeof(T) == 8)
        return std::bit_cast<T>(bswap_64(std::bit_cast<U>(x)));
#elif defined(_MSC_VER)
    if constexpr (sizeof(T) == 2)
        return std::bit_cast<T>(_byteswap_ushort(std::bit_cast<U>(x)));
    if constexpr (sizeof(T) == 4)
        return std::bit_cast<T>(_byteswap_ulong(std::bit_cast<U>(x)));
    if constexpr (sizeof(T) == 8)
        return std::bit_cast<T>(_byteswap_uint64(std::bit_cast<U>(x)));
#else
    return detail_constexpr::bswap(x);
#endif
}

}  // namespace openmsg
