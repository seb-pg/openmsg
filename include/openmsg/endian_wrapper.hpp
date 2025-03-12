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

#include "openmsg/optionull.hpp"
#include "openmsg/memory_wrapper.hpp"
#include "openmsg/user_definitions.hpp"

#include <type_traits>

namespace openmsg {

template<typename T>
concept endian_wrappable = serialisable<T> || optionullable<T>;

namespace detail {

template<endian_wrappable T, bool = optionullable<T>>
struct EndianWrapperBase
{
    using value_type = typename T::value_type;
    constexpr static value_type nullValue = T::nullValue;
};

template<serialisable T>
struct EndianWrapperBase<T, false>
{
    using value_type = T;
};

}  // namespace detail

#pragma pack(push, 1)

template<endian_wrappable T, template<typename...> class MemoryWrapper, std::endian Endian>
struct EndianWrapper : detail::EndianWrapperBase<T>
{
    using value_type = typename detail::EndianWrapperBase<T>::value_type;
    using storage_type = std::conditional_t<Endian == std::endian::native, value_type, as_uint_type_t<value_type>>;
    using memory_wrapper = MemoryWrapper<value_type, storage_type, std::integral_constant<std::endian, Endian>>;

    constexpr EndianWrapper() noexcept
    {
        if constexpr (optionullable<T>)
            value = memory_wrapper::htom(T::nullValue);
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

    // access to storage_value (should only be used for testing)
    constexpr const storage_type& storage_value() const noexcept
    {
        return value;
    }

private:
    storage_type value;
};

#pragma pack(pop)

template<endian_wrappable T>
using BigEndian    = EndianWrapper<T, endian_wrapper_user, std::endian::big>;

template<endian_wrappable T>
using LittleEndian = EndianWrapper<T, endian_wrapper_user, std::endian::little>;

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
