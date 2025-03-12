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

#include "openmsg/memory_wrapper.hpp"
#include "openmsg/user_definitions.hpp"

#include <algorithm>
#include <array>
#include <compare>
#include <limits>
#include <string>
#include <string_view>
#include <type_traits>

namespace openmsg {

// ArrayCharacter does not deal with endiancess intentionally (e.g. UTF16 and UTF32 are prohibited)

namespace detail_constexpr {

template<typename _T, size_t _N>
constexpr size_t array_len(const _T(&src)[_N])
{
    size_t i;
    for (i = 0; i < _N; ++i)
        if (src[i] == 0)
            break;
    return i;
}

template<typename _T, size_t _N, size_t _Ns>
constexpr void array_copy(_T(&elems)[_N],  const _T(&src_elems)[_Ns], bool is_zero_terminated)
{
    constexpr auto m = std::min(_N, _Ns);
    std::copy_n(src_elems, m, elems);
    if constexpr (m < _N)
        std::fill_n(elems + m, _N - m, static_cast<_T>(0));
    if (is_zero_terminated)
        elems[_N - 1] = 0;
}

template<typename _T, size_t _N, size_t _Ns>
constexpr std::strong_ordering array_strncmp(const _T(&elems)[_N], const _T(&src_elems)[_Ns])
{
    constexpr auto m = std::min(_N, _Ns);
    size_t i = 0;
    while (i < m)
    {
        auto diff = elems[i] <=> src_elems[i];
        if (diff != std::strong_ordering::equal)
            return diff;
        ++i;
    }
    return std::strong_ordering::equal;
}

}  // namespace detail_constexpr

template <class _T, class... _Types>
constexpr bool is_any_of = std::disjunction_v<std::is_same<_T, _Types>...>;

template<typename _T>
concept characters = is_any_of<_T, char, char8_t>;

template<characters _T, size_t _N, bool _is_zero_terminated>
requires (sizeof(_T) == 1)
struct ArrayCharacter
{
public:

    constexpr static size_t size = _N;
    constexpr static bool is_zero_terminated = _is_zero_terminated;
    using value_type = _T;

    constexpr ArrayCharacter(const ArrayCharacter& src) noexcept = default;
    constexpr ArrayCharacter& operator=(const ArrayCharacter& src) noexcept = default;
    constexpr ArrayCharacter(ArrayCharacter&& src) noexcept = default;
    constexpr ArrayCharacter& operator=(ArrayCharacter&& src) noexcept = default;

    // ctor

    constexpr ArrayCharacter(value_type x = 0) noexcept
    {
        std::fill_n(elems, size, x);
        if constexpr (is_zero_terminated)
            elems[size - 1] = 0;
    }

    template<size_t _Ns>
    constexpr ArrayCharacter(const value_type(&src_elems)[_Ns]) noexcept
    {
        detail_constexpr::array_copy(elems, src_elems, is_zero_terminated);
    }

    template<size_t _Ns, bool _Zs>
    constexpr ArrayCharacter(const ArrayCharacter<value_type, _Ns, _Zs>& src) noexcept
    {
        detail_constexpr::array_copy(elems, src.elems, is_zero_terminated);
    }

    // operator=

    template<size_t _Ns>
    constexpr ArrayCharacter& operator=(const value_type(&src_elems)[_Ns]) noexcept
    {
        detail_constexpr::array_copy(elems, src_elems, is_zero_terminated);
        return *this;
    }

    template<size_t _Ns, bool _Zs>
    constexpr ArrayCharacter& operator=(const ArrayCharacter<value_type, _Ns, _Zs>& src) noexcept
    {
        detail_constexpr::array_copy(elems, src.elems, is_zero_terminated);
        return *this;
    }

    // operator<=>, operator==

    template<size_t _Ns, bool izt>
    constexpr std::strong_ordering operator<=>(const ArrayCharacter<value_type, _Ns, izt>& rhs) const noexcept
    {
        return detail_constexpr::array_strncmp(elems, rhs.elems);
    }

    template<size_t _Ns, bool izt>
    constexpr bool operator==(const ArrayCharacter<value_type, _Ns, izt>& rhs) const noexcept
    {
        return operator<=>(rhs) == std::strong_ordering::equal;
    }

    // views

    constexpr std::basic_string_view<value_type> operator()() const noexcept
    {
        return to_string_view(true);
    }

    constexpr auto to_string_view(bool trim = true) const noexcept
    {
        return std::basic_string_view<value_type>(elems, length(trim));
    }

    // misc

    constexpr bool empty() const noexcept
    {
        return elems[0] == 0;
    }

    constexpr size_t length(bool trim = true) const noexcept
    {
        return trim ? detail_constexpr::array_len(elems) : size;
    }

    void clear() noexcept
    {
        std::fill_n(elems, size, static_cast<value_type>(0));
    }

    // misc (linked to from_array_pointer)

    ArrayCharacter(const std::basic_string_view<value_type>& src) noexcept
    {
        from_array_pointer(&*src.data(), src.size());
    }

    ArrayCharacter& operator=(const std::basic_string_view<value_type>& src) noexcept
    {
        from_array_pointer(&*src.data(), src.size());
        return *this;
    }

    ArrayCharacter& from_array_pointer(const value_type* src_elems, size_t max_size = std::numeric_limits<size_t>::max()) noexcept
    {
        // function name is intentionally long
        max_size = std::min(max_size, size);
        size_t i = 0;
        for ( ; i < max_size && *src_elems != 0 ; ++i)
            elems[i] = src_elems[i];
        std::fill_n(elems + i, size - i, static_cast<value_type>(0));
        if (is_zero_terminated)
            elems[size - 1] = 0;
        return *this;
    }

    value_type elems[size];
};

template<size_t _N, bool _is_zero_terminated = false>
using ArrayChar = ArrayCharacter<char, _N, _is_zero_terminated>;

template<size_t _N, bool _is_zero_terminated = false>
using ArrayChar8 = ArrayCharacter<char8_t, _N, _is_zero_terminated>;

}  // namespace openmsg
