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

#include "openmsg/bounds.hpp"
#include "openmsg/presence.hpp"

#include <bit>
#include <concepts>
#include <cstddef>
#include <inttypes.h>
#include <type_traits>

namespace openmsg {

template<swappable T,
         Presence _presence = Presence::required,
         T _nullValue = bounds<T>::nullValue,
         T _minValue = bounds<T>::minValue,
         T _maxValue = bounds<T>::maxValue>
struct Attributes
{
    // This is for a single value (does not use length)
    using value_type = T;
    constexpr static Presence presence = _presence;
    constexpr static T nullValue = _nullValue;
    constexpr static T minValue = _minValue;
    constexpr static T maxValue = _maxValue;
    constexpr static bool is_optional = Attributes::presence == Presence::optional;  // helper
};

template<typename T> concept has_attributes = requires(T a)
{
    requires swappable<typename T::value_type>;
    requires swappable<decltype(a.presence)>;
    requires std::is_same_v<decltype(a.presence), std::add_const_t<Presence>>;
    requires std::is_same_v<decltype(a.nullValue), decltype(a.minValue)>;
    requires std::is_same_v<decltype(a.nullValue), decltype(a.maxValue)>;
    requires std::is_same_v<decltype(a.minValue), decltype(a.maxValue)>;
    requires std::is_same_v<decltype(a.is_optional), std::add_const_t<bool>>;
};

}  // namespace openmsg
