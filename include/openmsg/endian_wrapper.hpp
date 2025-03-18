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


// concepts used locally

template<typename T> concept type_wrapper = requires(T a)
{
    requires swappable<typename T::value_type>;
    requires has_attributes<T>;
    requires has_attributes<typename T::attributes>;
};

template<typename T> concept wrappable = swappable<T> || type_wrapper<T>;

// local type_traits

template<typename T> struct underlying_type;
template<swappable T> struct underlying_type<T>
{
    using type = T;
    using attributes = Attributes<T>;
};
template<type_wrapper T> struct underlying_type<T>
{
    using type = typename T::value_type;
    using attributes = typename T::attributes;
};

template<typename T> using underlying_type_t = typename underlying_type<T>::type;
template<typename T> using underlying_attributes = typename underlying_type<T>::attributes;

// EndianWrapper

#pragma pack(push, 1)

template<wrappable T, std::endian _endian, template<typename H, std::endian> class MemoryWrapper = endian_wrapper_user>
struct EndianWrapper : underlying_attributes<T>
{
    using value_type = underlying_type_t<T>;
    using attributes = underlying_attributes<T>;
    using memory_wrapper = MemoryWrapper<value_type, _endian>;
    using memory_type = typename memory_wrapper::memory_type;
    constexpr static auto endian = _endian;
    constexpr static bool is_optional = EndianWrapper::presence == Presence::optional;

    constexpr EndianWrapper() noexcept
    {
        if constexpr (is_optional)
            value = memory_wrapper::htom(T::nullValue);
        else
            value = {};
    }

    // htom (HostType to MemoryType)
    constexpr EndianWrapper(const value_type& x) noexcept
        : value(memory_wrapper::htom(x))
    {
    }

    // mtoh (MemoryType to Memory)
    constexpr value_type operator()() const noexcept
    {
        return memory_wrapper::mtoh(value);
    }

    constexpr operator value_type() const noexcept
    {
        return operator()();
    }

    // access to storage_value (should only be used for testing)
    constexpr auto as_typed_value() const noexcept
    {
        return Type((*this)());
    }

    // access to storage_value (should only be used for testing)
    constexpr const memory_type& storage_value() const noexcept
    {
        return value;
    }

private:
    memory_type value;
};

#pragma pack(pop)

// BigEndian and LittleEndian
template<wrappable T> using BigEndian    = EndianWrapper<T, std::endian::big>;
template<wrappable T> using LittleEndian = EndianWrapper<T, std::endian::little>;

// Helpers

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
