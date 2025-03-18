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

#include "openmsg/attributes.hpp"
#include "openmsg/bounds.hpp"
#include "openmsg/concepts.hpp"
#include "openmsg/presence.hpp"

#include <bit>
#include <cstddef>
#include <inttypes.h>
#include <type_traits>

namespace openmsg {

template<swappable T, has_attributes _attributes = Attributes<T>>
struct Type : _attributes
{
    using value_type = T;
    using attributes = _attributes;
    constexpr static bool is_optional = Type::presence == Presence::optional;  // helper

    constexpr Type() noexcept = default;

    constexpr Type(const value_type& x) noexcept
        : value(x)
    {
    }

    constexpr value_type operator()() const noexcept
    {
        return value;
    }

    constexpr bool is_not_set() const noexcept
    {
        using U = as_uint_type_t<T>;
        return Type::is_optional && (std::bit_cast<U>(value) == std::bit_cast<U>(Type::nullValue));
    }

    constexpr bool in_bound() const noexcept
    {
        if constexpr (std::is_enum_v<value_type>)
        {
            using U = std::underlying_type_t<value_type>;
            return (static_cast<U>(Type::minValue) <= static_cast<U>(value)) && (static_cast<U>(value) <= static_cast<U>(Type::maxValue));
        }
        else
            return (Type::minValue <= value) && (value <= Type::maxValue);
    }

    value_type value = Type::is_optional ? Type::nullValue : value_type{};
};

}  // namespace openmsg
