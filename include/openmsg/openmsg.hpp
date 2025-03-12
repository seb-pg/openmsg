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

#include "openmsg/array_char.hpp"
#include "openmsg/attributes.hpp"
#include "openmsg/bounds.hpp"
#include "openmsg/bswap.hpp"
#include "openmsg/endian_wrapper.hpp"
#include "openmsg/memory_wrapper.hpp"
#include "openmsg/optionull.hpp"
#include "openmsg/type_traits.hpp"
#include "openmsg/user_definitions.hpp"
