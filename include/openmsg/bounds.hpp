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

#include <concepts>
#include <limits>

namespace openmsg {

template<typename T> struct bounds;

template<any_character T> struct bounds<T>
{
    static constexpr T minValue = static_cast<T>(0x20);
    static constexpr T maxValue = static_cast<T>(0xFF);
    static constexpr T nullValue = static_cast<T>(0x00);
};

template<signed_integral T> struct bounds<T>
{
    static constexpr T minValue = static_cast<T>(std::numeric_limits<T>::min() + 1);
    static constexpr T maxValue = std::numeric_limits<T>::max();
    static constexpr T nullValue = std::numeric_limits<T>::min();
};

template<unsigned_integral T> struct bounds<T>
{
    static constexpr T minValue = std::numeric_limits<T>::min();
    static constexpr T maxValue = static_cast<T>(std::numeric_limits<T>::max() - 1);
    static constexpr T nullValue = std::numeric_limits<T>::max();
};

template<std::floating_point T> struct bounds<T>
{
    static constexpr T minValue = -std::numeric_limits<T>::max();
    static constexpr T maxValue = std::numeric_limits<T>::max();
    static constexpr T nullValue = std::numeric_limits<T>::quiet_NaN();
};

template<enumerated T> struct bounds<T>
{
    using U = std::underlying_type_t<T>;
    static constexpr T minValue = static_cast<T>(bounds<U>::minValue);
    static constexpr T maxValue = static_cast<T>(bounds<U>::maxValue);
    static constexpr T nullValue = static_cast<T>(bounds<U>::nullValue);
};

}  // namespace openmsg
