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

#include "openmsg/optionull.h"
#include "openmsg/memory_wrapper.h"
#include "openmsg/user_definitions.h"

namespace openmsg {

template<typename _T>
concept endian_wrappable = serialisable<_T> || optionullable<_T>;

namespace detail {

template<endian_wrappable _T, bool = optionullable<_T>>
struct EndianWrapperBase
{
    using value_type = typename _T::value_type;
    constexpr static value_type nullValue = _T::nullValue;
};

template<serialisable _T>
struct EndianWrapperBase<_T, false>
{
    using value_type = _T;
};

}  // namespace detail

#pragma pack(push, 1)

template<endian_wrappable _T, template<typename...> class _memory_wrapper, std::endian _endian>
struct EndianWrapper : detail::EndianWrapperBase<_T>
{
    using value_type = typename detail::EndianWrapperBase<_T>::value_type;
    using storage_type = std::conditional_t<_endian == std::endian::native, value_type, as_uint_type_t<value_type>>;
    using memory_wrapper = _memory_wrapper<value_type, storage_type, std::integral_constant<std::endian, _endian>>;

    constexpr EndianWrapper() noexcept
    {
        if constexpr (optionullable<_T>)
            value = memory_wrapper::htom(_T::nullValue);
        else if (std::is_constant_evaluated())
            value = {};
    }

    // htom
    constexpr EndianWrapper(const value_type& x) noexcept
        : value(memory_wrapper::htom(x))
    {
    }

    // mtoh
    constexpr value_type operator()() const noexcept
    {
        return memory_wrapper::mtoh(value);
    }

    constexpr operator value_type() const noexcept
    {
        return operator()();
    }

private:
    storage_type value;
};

#pragma pack(pop)

template<endian_wrappable _T>
using BigEndian    = EndianWrapper<_T, endian_wrapper_user, std::endian::big>;

template<endian_wrappable _T>
using LittleEndian = EndianWrapper<_T, endian_wrapper_user, std::endian::little>;

using le_int8_t = LittleEndian<uint8_t>;
using le_uint8_t = LittleEndian<uint8_t>;
using le_int16_t = LittleEndian<int16_t>;
using le_uint16_t = LittleEndian<uint16_t>;
using le_int32_t = LittleEndian<int32_t>;
using le_uint32_t = LittleEndian<uint32_t>;
using le_int64_t = LittleEndian<int64_t>;
using le_uint64_t = LittleEndian<uint64_t>;
using le_float_t = LittleEndian<float>;
using le_double_t = LittleEndian<double>;

using be_int8_t = BigEndian<int8_t>;
using be_uint8_t = BigEndian<uint8_t>;
using be_int16_t = BigEndian<int16_t>;
using be_uint16_t = BigEndian<uint16_t>;
using be_int32_t = BigEndian<int32_t>;
using be_uint32_t = BigEndian<uint32_t>;
using be_int64_t = BigEndian<int64_t>;
using be_uint64_t = BigEndian<uint64_t>;
using be_float_t = BigEndian<float>;
using be_double_t = BigEndian<double>;

}  // namespace openmsg

#endif
