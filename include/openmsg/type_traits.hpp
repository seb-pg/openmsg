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

#include <bit>
#include <inttypes.h>
#include <type_traits>

namespace openmsg {

template <class T, class... Types>
constexpr bool is_any_of = std::disjunction_v<std::is_same<T, Types>...>;

template<typename T>
requires (sizeof(T) <= 8 && std::has_single_bit(sizeof(T)))
using as_uint_type_t =  std::conditional_t<sizeof(T) == 8, uint64_t,
                        std::conditional_t<sizeof(T) == 4, uint32_t,
                        std::conditional_t<sizeof(T) == 2, uint16_t, uint8_t>>>;

template<typename T>
requires (sizeof(T) <= 8 && std::has_single_bit(sizeof(T)))
using as_half_size_t =  std::conditional_t<sizeof(T) == 8, uint32_t,
                        std::conditional_t<sizeof(T) == 4, uint16_t, uint8_t>>;

using ascii8_t = unsigned char;

}  // namespace openmsg
