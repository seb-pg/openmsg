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
#include "openmsg/type_traits.hpp"
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

template<typename T, size_t N>
constexpr size_t array_len(const T(&src)[N]) noexcept
{
    size_t i;
    for (i = 0; i < N; ++i)
        if (src[i] == 0)
            break;
    return i;
}

template<typename T, size_t N, typename S, size_t Ns>
constexpr void array_copy(T(&elems)[N],  const S(&src_elems)[Ns], bool is_zero_terminated) noexcept
{
    constexpr auto m = std::min(N, Ns);
    std::copy_n(src_elems, m, elems);
    if constexpr (m < N)
        std::fill_n(elems + m, N - m, static_cast<T>(0));
    if (is_zero_terminated)
        elems[N - 1] = 0;
}

template<typename T, size_t N, size_t Ns>
constexpr std::strong_ordering array_strncmp(const T(&elems)[N], const T(&src_elems)[Ns]) noexcept
{
    constexpr auto m = std::min(N, Ns);
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

template<typename T, size_t N, bool IsZeroTerminated>
requires (sizeof(T) == 1) && is_any_of<T, char, char8_t>
struct ArrayCharacter
{
public:
    constexpr static size_t size = N;
    constexpr static bool is_zero_terminated = IsZeroTerminated;
    using value_type = T;

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

    template<size_t Ns, typename input_type>
    constexpr ArrayCharacter(const input_type(&src_elems)[Ns]) noexcept
    {
        detail_constexpr::array_copy(elems, src_elems, is_zero_terminated);
    }


    template<size_t Ns, bool Zs>
    constexpr ArrayCharacter(const ArrayCharacter<value_type, Ns, Zs>& src) noexcept
    {
        detail_constexpr::array_copy(elems, src.elems, is_zero_terminated);
    }

    // operator=

    template<size_t Ns, typename input_type>
    constexpr ArrayCharacter& operator=(const input_type(&src_elems)[Ns]) noexcept
    {
        detail_constexpr::array_copy(elems, src_elems, is_zero_terminated);
        return *this;
    }

    template<size_t Ns, bool Zs>
    constexpr ArrayCharacter& operator=(const ArrayCharacter<value_type, Ns, Zs>& src) noexcept
    {
        detail_constexpr::array_copy(elems, src.elems, is_zero_terminated);
        return *this;
    }

    // operator<=>, operator==

    template<size_t Ns, bool IsZeroTerminatedS>
    constexpr std::strong_ordering operator<=>(const ArrayCharacter<value_type, Ns, IsZeroTerminatedS>& rhs) const noexcept
    {
        return detail_constexpr::array_strncmp(elems, rhs.elems);
    }

    template<size_t Ns, bool IsZeroTerminatedS>
    constexpr bool operator==(const ArrayCharacter<value_type, Ns, IsZeroTerminatedS>& rhs) const noexcept
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

    template<typename input_type>
    ArrayCharacter(const std::basic_string_view<input_type>& src) noexcept
    {
        from_array_pointer(&*src.data(), src.size());
    }

    template<typename input_type>
    ArrayCharacter& operator=(const std::basic_string_view<input_type>& src) noexcept
    {
        from_array_pointer(&*src.data(), src.size());
        return *this;
    }

    template<typename input_type>
    ArrayCharacter& from_array_pointer(const input_type* src_elems, size_t max_size = std::numeric_limits<size_t>::max()) noexcept
    {
        // function name is intentionally long
        max_size = std::min(max_size, size);
        size_t i = 0;
        for ( ; i < max_size && *src_elems != 0 ; ++i)
            elems[i] = std::bit_cast<value_type>(src_elems[i]);
        std::fill_n(elems + i, size - i, static_cast<value_type>(0));
        if (is_zero_terminated)
            elems[size - 1] = 0;
        return *this;
    }

    value_type elems[size];
};

template<size_t N, bool IsZeroTerminated = false>
using ArrayChar = ArrayCharacter<char, N, IsZeroTerminated>;

template<size_t N, bool IsZeroTerminated = false>
using ArrayChar8 = ArrayCharacter<char8_t, N, IsZeroTerminated>;

}  // namespace openmsg
