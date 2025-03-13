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
constexpr T bswap(T x) noexcept
{
    if constexpr (sizeof(T) == 1)
        return x;
    using U = std::make_unsigned_t<T>;
    using H = as_half_size_t<T>;
    auto y = static_cast<U>(x);
    constexpr U shift = sizeof(T) * 4;
    auto msw = bswap(static_cast<H>(y));
    auto lsw = bswap(static_cast<H>(y >> shift));
    return static_cast<T>((static_cast<U>(msw) << shift) | static_cast<U>(lsw));
}

}  // namespace detail_constexpr

template<std::integral T>
constexpr T bswap(T x) noexcept
{
    if (std::is_constant_evaluated())
        return detail_constexpr::bswap(x);
    if constexpr (sizeof(T) == 1)
        return x;
#if defined(__GNUC__) || defined(__clang__)
    if constexpr (sizeof(T) == 2)
        return bswap_16(x);
    if constexpr (sizeof(T) == 4)
        return bswap_32(x);
    if constexpr (sizeof(T) == 8)
        return bswap_64(x);
#elif defined(_MSC_VER)
    if constexpr (sizeof(T) == 2)
        return _byteswap_ushort(x);
    if constexpr (sizeof(T) == 4)
        return _byteswap_ulong(x);
    if constexpr (sizeof(T) == 8)
        return _byteswap_uint64(x);
#else
    return detail_constexpr::bswap(x);
#endif
}

}  // namespace openmsg
