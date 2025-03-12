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

#include <concepts>
#include <limits>
#include <type_traits>

namespace openmsg {

template<typename T>
concept serialisable = std::is_arithmetic_v<T> || std::is_enum_v<T>;

template<typename T>
requires std::is_arithmetic_v<T>
struct OptionullDefault {

constexpr static T null =  std::is_floating_point_v<T> ?
                            std::numeric_limits<T>::quiet_NaN() :
                            (std::is_signed_v<T> ? std::numeric_limits<T>::min() : std::numeric_limits<T>::max());
constexpr static T min = static_cast<T>(std::numeric_limits<T>::min() + (std::is_signed_v<T> ? 1 : 0));
constexpr static T max = static_cast<T>(std::numeric_limits<T>::max() - (std::is_signed_v<T> ? 0 : 1));


};

template<serialisable T, T NullValue = OptionullDefault<T>::null>
struct Optionull
{
    using value_type = T;
    constexpr static value_type nullValue = NullValue;

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
    value_type value = NullValue;
};

template<typename T>
concept optionullable = requires(T x)
{
    requires std::is_same_v<std::add_const_t<typename T::value_type>, decltype(x.nullValue)>;
};

}  // namespace openmsg
