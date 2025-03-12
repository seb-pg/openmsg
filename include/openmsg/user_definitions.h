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

#include "openmsg/memory_wrapper.h"

namespace openmsg {

// openmsg does not take side with regards to CPU capabilities, therefore, the
// user must defined endian_wrapper_user in include/openmsg/user_definitions.h

template<typename _H, typename _M, typename _E>
using endian_wrapper_user = memory_wrapper_bswap<_H, _M, _E>;

}  // namespace openmsg

#endif
