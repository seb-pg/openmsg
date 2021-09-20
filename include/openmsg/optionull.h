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

#include <concepts>
#include <limits>

namespace openmsg {

template<typename _T>
concept serialisable = std::is_arithmetic_v<_T> || std::is_enum_v<_T>;

template<typename _T>
requires std::is_arithmetic_v<_T>
struct OptionullDefault {

constexpr static _T null =  std::is_floating_point_v<_T> ?
                            std::numeric_limits<_T>::quiet_NaN() :
                            (std::is_signed_v<_T> ? std::numeric_limits<_T>::min() : std::numeric_limits<_T>::max());
constexpr static _T min = static_cast<_T>(std::numeric_limits<_T>::min() + (std::is_signed_v<_T> ? 1 : 0));
constexpr static _T max = static_cast<_T>(std::numeric_limits<_T>::max() - (std::is_signed_v<_T> ? 0 : 1));


};

template<serialisable _T, _T _nullValue = OptionullDefault<_T>::null>
struct Optionull
{
    using value_type = _T;
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
    value_type value = _nullValue;
};

template<typename _T>
concept optionullable = requires(_T x)
{
    requires std::is_same_v<std::add_const_t<typename _T::value_type>, decltype(x.nullValue)>;
};

}  // namespace openmsg

#endif
