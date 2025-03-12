// openmsg by Sebastien Rubens
//
// To the extent possible under law, the person who associated CC0 with
// openmsg has waived all copyright and related or neighboring rights
// to openmsg.
//
// You should have received a copy of the CC0 legalcode along with this
// work.  If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.

#pragma once
#include <type_traits>
#if (__cplusplus >= 202002L) || defined(_HAS_CXX20)

#include "openmsg/bswap.h"

#include <inttypes.h>
#include <bit>
#include <concepts>

#if defined(__GNUC__) || defined(__clang__)
#include <byteswap.h>
#elif defined(_MSC_VER)
#include <stdlib.h>
#include "immintrin.h"
#endif

namespace openmsg {

// openmsg does not take side with regards to memory aliasing and
// memory access reordering issues (either software or hardware)

template<typename _T>
requires (sizeof(_T) <= 8 && std::has_single_bit(sizeof(_T)))
using as_uint_type_t =  std::conditional_t<sizeof(_T) == 8, uint64_t,
                        std::conditional_t<sizeof(_T) == 4, uint32_t,
                        std::conditional_t<sizeof(_T) == 2, uint16_t, uint8_t>>>;

template<typename _H, typename _M, typename _E>
struct memory_wrapper_bswap
{
    constexpr static auto endian = _E::value;
    using _U = as_uint_type_t<_H>;

    constexpr static _H mtoh(const _M& x) noexcept
    {
        auto y = std::bit_cast<_U>(x);
        if constexpr (endian != std::endian::native)
            y = bswap(y);
        return std::bit_cast<_H>(y);
    }

    constexpr static _M htom(const _H& x) noexcept
    {
        auto y = std::bit_cast<_U>(x);
        if constexpr (endian != std::endian::native)
            y = bswap(y);
        return std::bit_cast<_M>(y);
    }
};

template<typename _H, typename _M, typename _E>
struct memory_wrapper_robust
{
    constexpr static auto endian = _E::value;
    using _U = as_uint_type_t<_H>;

    using bytes_t = uint8_t[sizeof(_H)];
    constexpr static int mask = endian == std::endian::native ? 0 : (sizeof(_H) - 1);

    constexpr static _H mtoh(const _M& x) noexcept
    {
        if (std::is_constant_evaluated())
            return memory_wrapper_bswap<_H, _M, _E>::mtoh(x);
        auto y = std::bit_cast<_U>(x);
        if constexpr (sizeof(_M) == 1 || endian == std::endian::native)
            return std::bit_cast<_H>(y);
        //
        _U dst;
        auto values = reinterpret_cast<const bytes_t&>(y);
        dst = static_cast<_U>(values[mask ^ 0]);
        if constexpr (sizeof(_U) >= 2)
            dst |= static_cast<_U>(values[mask ^ 1]) << 8ul;
        if constexpr (sizeof(_U) >= 4)
            dst |= static_cast<_U>(values[mask ^ 2]) << 16ul |
            static_cast<_U>(values[mask ^ 3]) << 24ul;
        if constexpr (sizeof(_U) >= 8)
            dst |= static_cast<_U>(values[mask ^ 4]) << 32ull |
                   static_cast<_U>(values[mask ^ 5]) << 40ull |
                   static_cast<_U>(values[mask ^ 6]) << 48ull |
                   static_cast<_U>(values[mask ^ 7]) << 56ull;
        return std::bit_cast<_H>(dst);
    };

    constexpr static _M htom(const _H& x) noexcept
    {
        if (std::is_constant_evaluated())
            return memory_wrapper_bswap<_H, _M, _E>::htom(x);
        auto y = std::bit_cast<_U>(x);
        if constexpr (sizeof(_M) == 1 || endian == std::endian::native)
            return std::bit_cast<_M>(y);
        //
        _M dst;
        auto values = reinterpret_cast<bytes_t&>(dst);
        values[mask ^ 0] = static_cast<uint8_t>(y);
        if constexpr (sizeof(_U) >= 2)
            values[mask ^ 1] = static_cast<uint8_t>(y >> 8ul);
        if constexpr (sizeof(_U) >= 4)
        {
            values[mask ^ 2] = static_cast<uint8_t>(y >> 16ul);
            values[mask ^ 3] = static_cast<uint8_t>(y >> 24ul);
        }
        if constexpr (sizeof(_U) >= 8)
        {
            values[mask ^ 4] = static_cast<uint8_t>(y >> 32ul);
            values[mask ^ 5] = static_cast<uint8_t>(y >> 40ul);
            values[mask ^ 6] = static_cast<uint8_t>(y >> 48ul);
            values[mask ^ 7] = static_cast<uint8_t>(y >> 56ul);
        }
        return dst;
    };
};

template<typename _H, typename _M, typename _E>
struct memory_wrapper_movbe
{
    // for Intel CPU of 4th generation Intel® Core™ processor family (codenamed Haswell)
    constexpr static auto endian = _E::value;
    using _U = as_uint_type_t<_H>;

    constexpr static _H mtoh(const _M& x) noexcept
    {
        if (std::is_constant_evaluated())
            return memory_wrapper_bswap<_H, _M, _E>::mtoh(x);
        auto y = std::bit_cast<_U>(x);
        if constexpr (sizeof(_M) == 1 || endian == std::endian::native)
            return std::bit_cast<_H>(y);
        //
#if defined(_MSC_VER)
        if constexpr (sizeof(_M) == 2)
            return std::bit_cast<_H>(_load_be_u16(&y));
        if constexpr (sizeof(_M) == 4)
            return std::bit_cast<_H>(_load_be_u32(&y));
        if constexpr (sizeof(_M) == 8)
            return std::bit_cast<_H>(_load_be_u64(&y));
#else
        return memory_wrapper_bswap<_H, _M, _E>::mtoh(x);
#endif
    }

    constexpr static _M htom(const _H& x) noexcept
    {
        if (std::is_constant_evaluated())
            return memory_wrapper_bswap<_H, _M, _E>::htom(x);
        auto y = std::bit_cast<_U>(x);
        if constexpr (sizeof(_M) == 1 || endian == std::endian::native)
            return std::bit_cast<_M>(y);
        //
#if defined(_MSC_VER)
        _M dst;
        if constexpr (sizeof(_M) == 2)
            _store_be_u16(&dst, y);
        if constexpr (sizeof(_M) == 4)
            _store_be_u32(&dst, y);
        if constexpr (sizeof(_M) == 8)
            _store_be_u64(&dst, y);
        return dst;
#else
        return memory_wrapper_bswap<_H, _M, _E>::htom(x);
#endif
    }
};

}  // namespace openmsg

#endif
