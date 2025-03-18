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

#include "openmsg/type_traits.hpp"

#include <inttypes.h>
#include <concepts>
#include <type_traits>

namespace openmsg {

template<typename T> concept any_character     = is_any_of<T, char8_t>;
template<typename T> concept signed_integral   = is_any_of<T, int8_t, int16_t, int32_t, int64_t>;
template<typename T> concept unsigned_integral = is_any_of<T, uint8_t, uint16_t, uint32_t, uint64_t>;
template<typename T> concept any_integral      = signed_integral<T> || unsigned_integral<T>;
template<typename T> concept enumerated        = std::is_enum_v<T>;
template<typename T> concept swappable         = any_character<T> || any_integral<T> || std::floating_point<T> || enumerated<T>;

}  // namespace openmsg
