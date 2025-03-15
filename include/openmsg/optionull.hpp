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
#include "openmsg/concepts.hpp"

#include <limits>
#include <type_traits>

namespace openmsg {

template<swappable T, T _nullValue = bounds<T>::nullValue>
struct Optionull
{
    using value_type = T;
    constexpr static value_type nullValue = _nullValue;

    constexpr Optionull() noexcept = default;

    // htom
    constexpr Optionull(const value_type& x) noexcept
        : value(x)
    {
    }

    // mtoh
    constexpr value_type operator()() const noexcept
    {
        return value;
    }

private:
    value_type value = Optionull::nullValue;
};

}  // namespace openmsg
