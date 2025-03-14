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

#include "openmsg/concepts.hpp"
#include "openmsg/optionull.hpp"
#include "openmsg/memory_wrapper.hpp"
#include "openmsg/user_definitions.hpp"

#include <type_traits>

namespace openmsg {

namespace detail {

template<typename T>
struct EndianWrapperBase
{
    using value_type = T;
};

template<optionullable T>
struct EndianWrapperBase<T>
{
    using value_type = typename T::value_type;
    constexpr static value_type nullValue = T::nullValue;
};

}  // namespace detail

#pragma pack(push, 1)

template<swappable T,
         template<typename H, std::endian _endian> class MemoryWrapper,
         std::endian _endian>
struct EndianWrapper : detail::EndianWrapperBase<T>
{
    using value_type = typename detail::EndianWrapperBase<T>::value_type;
    using memory_wrapper = MemoryWrapper<value_type, _endian>;
    using storage_type = typename memory_wrapper::storage_type;

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

template<swappable T>
using BigEndian    = EndianWrapper<T, endian_wrapper_user, std::endian::big>;

template<swappable T>
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
