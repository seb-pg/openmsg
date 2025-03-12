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

template<std::integral _T>
constexpr _T bswap(_T x) noexcept
{
    if constexpr (sizeof(_T) == 1)
        return x;
    using _U = std::make_unsigned_t<_T>;
    auto y = static_cast<_U>(x);
    constexpr _U shift = sizeof(_T) * 4;
    using _H = std::conditional_t<sizeof(_T) == 8, uint32_t, std::conditional_t<sizeof(_T) == 4, uint16_t, uint8_t>>;
    auto msw = bswap(static_cast<_H>(y));
    auto lsw = bswap(static_cast<_H>(y >> shift));
    return static_cast<_T>((static_cast<_U>(msw) << shift) | static_cast<_U>(lsw));
}

}  // namespace detail_constexpr

template<std::integral _T>
constexpr _T bswap(_T x) noexcept
{
    if (std::is_constant_evaluated())
        return detail_constexpr::bswap(x);
    if constexpr (sizeof(_T) == 1)
        return x;
#if defined(__GNUC__) || defined(__clang__)
    if constexpr (sizeof(_T) == 2)
        return bswap_16(x);
    if constexpr (sizeof(_T) == 4)
        return bswap_32(x);
    if constexpr (sizeof(_T) == 8)
        return bswap_64(x);
#elif defined(_MSC_VER)
    if constexpr (sizeof(_T) == 2)
        return _byteswap_ushort(x);
    if constexpr (sizeof(_T) == 4)
        return _byteswap_ulong(x);
    if constexpr (sizeof(_T) == 8)
        return _byteswap_uint64(x);
#else
    return detail_constexpr::bswap(x);
#endif
}

}  // namespace openmsg
