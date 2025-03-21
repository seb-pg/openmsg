// openmsg by Sebastien Rubens
//
// To the extent possible under law, the person who associated CC0 with
// openmsg has waived all copyright and related or neighboring rights
// to openmsg.
//
// You should have received a copy of the CC0 legalcode along with this
// work.  If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.

#include "openmsg/bswap.hpp"
#include "openmsg/array_char.hpp"
#include "openmsg/concepts.hpp"
#include "openmsg/endian_wrapper.hpp"
#include "openmsg/memory_wrapper.hpp"
#include "openmsg/optionull.hpp"
#include "openmsg/type.hpp"

#include "inttypes.h"

#include <cassert>
#include <format>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>
#include <source_location>
#include <string.h>
#include <type_traits>
#include <vector>

namespace openmsg {

void __dynamic_assert(bool value, std::source_location loc)
{
    if (!bool(value))
        throw std::runtime_error(std::format("error in {} at line {}", loc.file_name(),  loc.line()));
}

#define dynamic_assert(value) __dynamic_assert((value), std::source_location::current())


template<typename T>
struct test_array
{
template<size_t N, bool is_zero_terminated = false>
using ArrayChar_t = ArrayCharacter<T, N, is_zero_terminated>;

static void test()
{
    // default ctor
    constexpr ArrayChar_t<5> a;  // filled with zeros by default
    static_assert(a.to_string_view(false).length() == a.size);
    static_assert(a.to_string_view(true).empty());
    static_assert(a().empty());
    static_assert(a.empty());
    constexpr ArrayChar_t<5> b('x');  // filled with 'x' by default
    static_assert(b.to_string_view(true).length() == b.size);

    // ctor from char[]
    constexpr ArrayChar_t<8> b1("testmefurther");  // non zero terminated
    constexpr ArrayChar_t<8, true> b2("testmefurther");  // zero terminated
    constexpr ArrayChar_t<8> b3(b2);
    static_assert(b1 != b2);
    static_assert(b2 == b3);
    static_assert(b1.to_string_view(false).length() == b1.length());
    static_assert(b2.to_string_view(true).length() == b2.length());
    static_assert(b2().length() == b2.length());

    // "copy" ctor
    constexpr ArrayChar_t<5> c1(b2);
    constexpr ArrayChar_t<15> c2(b2);
    constexpr ArrayChar_t<8, true> c3(b2);
    static_assert(c1.length() == c1.size);
    static_assert(c2.length() == b2.length());
    static_assert(c3.length() == b2.length());

    // operator=
    ArrayChar_t<5> d1;
    ArrayChar_t<5> d2(c1);
    d1 = c1;
    dynamic_assert(d1 == c1);
    dynamic_assert(d2 == c1);
    ArrayChar_t<15> d3;
    ArrayChar_t<15> d4("testmefurther");
    d3 = "testmefurther";
    dynamic_assert(d3 == d4);

    // misc
    ArrayChar_t<5> e1('x');
    ArrayChar_t<5> e2("xxxxxx");
    dynamic_assert(e1 == e2);
    e1.clear();
    dynamic_assert(e1.empty());

    // misc (linked to from_array_pointer)
    const std::string testmefurther = "testmefurther";
    const std::string_view testmefurther_sv = testmefurther;

    ArrayChar_t<5> f1(testmefurther_sv);

    ArrayChar_t<5> f2;
    f2 = testmefurther_sv;
    dynamic_assert(f1 == f2);

    ArrayChar_t<5> f3;
    f3.from_array_pointer(testmefurther.c_str());
    dynamic_assert(f1 == f3);
}
};


template<swappable T>
constexpr T simple_byteswap(T _input)
{
    // another way to do a byteswap, for testing purpose
    using U = std::make_unsigned_t<T>;
    auto input = std::bit_cast<U>(_input);
    U output = 0;
    for (size_t i=0 ; i<sizeof(U); ++i)
    {
        output = std::rotl(output, 8);
        output |= input & 0xFF;
        input = std::rotr(input, 8);
    }
    return std::bit_cast<T>(output);
}

template<swappable T, swappable P = T>
void test_primitive()
{
    using U = as_uint_type_t<T>;

    constexpr auto _constant = static_cast<uint64_t>(0x8091a2b3c4d5e6f7ull);
    constexpr auto intialisation_value = std::bit_cast<T>(static_cast<U>(_constant));

    // bswap
    constexpr auto constexpr_value = static_cast<U>(_constant);
    constexpr auto constexpr_swapped = bswap(constexpr_value);
    constexpr auto constexpr_expected = simple_byteswap(constexpr_value);
    static_assert(std::bit_cast<U>(constexpr_swapped) == std::bit_cast<U>(constexpr_expected));

    auto value = static_cast<U>(constexpr_value);
    auto swapped = bswap(value);
    auto expected = simple_byteswap(value);
    dynamic_assert(std::bit_cast<U>(swapped) == std::bit_cast<U>(expected));

    // swappable
    static_assert(swappable<T>);

    // check normal (type) defaulted value, default nullValue
    constexpr Type<T> tvalue_defaulted;
    static_assert(!tvalue_defaulted.is_optional);
    static_assert(std::bit_cast<U>(tvalue_defaulted.maxValue) == std::bit_cast<U>(bounds<T>::maxValue));
    static_assert(std::bit_cast<U>(tvalue_defaulted.minValue) == std::bit_cast<U>(bounds<T>::minValue));
    static_assert(std::bit_cast<U>(tvalue_defaulted.nullValue) == std::bit_cast<U>(bounds<T>::nullValue));

    // check normal (type) initialised value, default nullValue
    constexpr Type<T> tvalue_initialiased(intialisation_value);
    static_assert(std::bit_cast<U>(tvalue_initialiased()) == std::bit_cast<U>(intialisation_value));

    // check optional (Optionull) defaulted value, default nullValue
    constexpr Optionull<T> ovalue_defaulted;
    static_assert(ovalue_defaulted.is_optional);
    static_assert(std::bit_cast<U>(ovalue_defaulted.maxValue) == std::bit_cast<U>(bounds<T>::maxValue));
    static_assert(std::bit_cast<U>(ovalue_defaulted.minValue) == std::bit_cast<U>(bounds<T>::minValue));
    static_assert(std::bit_cast<U>(ovalue_defaulted.nullValue) == std::bit_cast<U>(bounds<T>::nullValue));

    // check optional (Optionull) initialised value, default nullValue
    constexpr Optionull<T> ovalue_initialiased(intialisation_value);
    static_assert(std::bit_cast<U>(ovalue_initialiased()) == std::bit_cast<U>(intialisation_value));

    // Check Type and Optionull attributes are accessible
    static_assert(has_attributes<Type<T>>);
    static_assert(has_attributes<typename Type<T>::attributes>);
    static_assert(has_attributes<Optionull<T>>);
    static_assert(has_attributes<typename Optionull<T>::attributes>);

    //
    static_assert(tvalue_initialiased.in_bound());
    static_assert(tvalue_defaulted.in_bound() || any_character<P>);
    static_assert(ovalue_initialiased.in_bound() || ovalue_initialiased.is_not_set());
    static_assert(ovalue_defaulted.in_bound() || ovalue_defaulted.is_not_set());
}

template<typename OurEndianWrapper, typename ... Args>
void test_endian(Args&&... args)
{
    using value_type = typename OurEndianWrapper::value_type;
    using unsigned_type = as_uint_type_t<value_type>;
    constexpr auto expected_null_value = static_cast<value_type>(123);
    constexpr bool is_optional = OurEndianWrapper::is_optional;

    static_assert(has_attributes<OurEndianWrapper>);
    static_assert(has_attributes<typename OurEndianWrapper::attributes>);

    OurEndianWrapper oe(std::forward<Args>(args)...);
    if constexpr (oe.is_optional)
        dynamic_assert(std::bit_cast<unsigned_type>(oe.nullValue) == std::bit_cast<unsigned_type>(expected_null_value));

    const auto value = std::bit_cast<unsigned_type>(oe());
    if (sizeof...(Args) > 0)
    {
        constexpr auto expected_value = static_cast<value_type>(17);
        dynamic_assert(std::bit_cast<unsigned_type>(value) == std::bit_cast<unsigned_type>(expected_value));
    }
    else if (is_optional)
    {
        dynamic_assert(std::bit_cast<unsigned_type>(value) == std::bit_cast<unsigned_type>(expected_null_value));
    }
    else
    {
        constexpr auto expected_zero_value = static_cast<value_type>(0);
        dynamic_assert(std::bit_cast<unsigned_type>(value) == std::bit_cast<unsigned_type>(expected_zero_value));
    }

    //
    constexpr size_t size = 8 + sizeof(OurEndianWrapper(std::forward<Args>(args)...));
    constexpr char unset_value = static_cast<char>(0xCCu);
    std::vector<char> buf(size, unset_value);  // buffer to create an OurEndian using placement new
    new (&*buf.begin()) OurEndianWrapper(std::forward<Args>(args)...);
    auto& obj = *new (&*buf.begin()) OurEndianWrapper(std::forward<Args>(args)...);

    auto host_value = obj();  // get the underlying value, converted to host endian

    value_type casted_host_value = obj;
    static_assert(std::is_same_v<decltype(casted_host_value), decltype(host_value)>);
    dynamic_assert(std::bit_cast<unsigned_type>(casted_host_value) == std::bit_cast<unsigned_type>(host_value));

    auto stored_value = obj.storage_value();
    static_assert(sizeof(obj) == sizeof(host_value));
    static_assert(sizeof(obj) == sizeof(stored_value));
    static_assert(std::is_same_v<value_type, decltype(host_value)>);
    static_assert(std::is_same_v<typename OurEndianWrapper::memory_type, decltype(stored_value)>);

    constexpr auto endian = OurEndianWrapper::endian;
    if constexpr (endian == std::endian::native)
        dynamic_assert(std::bit_cast<unsigned_type>(stored_value) == std::bit_cast<unsigned_type>(host_value));
    else
    {
        auto bswap_host_value = detail_constexpr::bswap(std::bit_cast<as_uint_type_t<value_type>>(host_value));
        dynamic_assert(std::bit_cast<unsigned_type>(stored_value) == std::bit_cast<unsigned_type>(bswap_host_value));
    }

    //
    auto typed_value = oe.as_typed_value();
    static_assert(type_wrapper<decltype(typed_value)>);
}

template<swappable T, std::endian endian, template<swappable, std::endian> class MemoryWrapper>
void test_memory_endian_value(const T& value)
{
    using memory_wrapper = MemoryWrapper<T, endian>;
    const auto memory_value = memory_wrapper::htom(value);
    const auto host_value = memory_wrapper::mtoh(memory_value);
    dynamic_assert(host_value == value);

    using DirectEndianWrapper = EndianWrapper<T, endian, MemoryWrapper>;
    test_endian<DirectEndianWrapper>();
    test_endian<DirectEndianWrapper>(value);

    constexpr auto nullValue = static_cast<T>(123);
    using OptionalEndianWrapper = EndianWrapper<Optionull<T, nullValue>, endian, MemoryWrapper>;
    static_assert(OptionalEndianWrapper::nullValue == static_cast<T>(123));
    test_endian<OptionalEndianWrapper>();
    test_endian<OptionalEndianWrapper>(value);
}

template<swappable T, std::endian endian>
void test_memory_endian()
{
    const auto value = static_cast<T>(17);
    test_memory_endian_value<T, endian, memory_wrapper_robust>(value);
    test_memory_endian_value<T, endian, memory_wrapper_bswap>(value);
    test_memory_endian_value<T, endian, memory_wrapper_movbe>(value);
}

template<swappable T>
void test_memory()
{
    test_memory_endian<T, std::endian::little>();
    test_memory_endian<T, std::endian::big>();
}

template<swappable T>
void test_type()
{
    test_primitive<T>();
    test_memory<T>();
    if constexpr (!std::floating_point<T>)
    {
        enum class enum_type : T {};
        test_primitive<enum_type, T>();
        test_memory<enum_type>();
    }
}

#pragma pack(push)
#pragma pack(1)

template<template<typename...> class _W>
struct test_message
{
    _W<uint8_t> a = 0xAFu;
    _W<Optionull<uint16_t>> b[2] = { 0x8091, 0x8091 };
    _W<Optionull<uint64_t>> c[2] = { 0x8091a2b3c4d5e6f7ull, 0x8091a2b3c4d5e6f7ull };
    ArrayChar<9> d = "testme";
    ArrayChar8<9> e = u8"testmefurther";
};

#pragma pack(pop)

void test_messages()
{
    constexpr test_message<BigEndian> mb;
    uint8_t mb_expected[] = {
        0xAF, 0x80, 0x91, 0x80, 0x91,
        0x80, 0x91, 0xa2, 0xb3, 0xc4, 0xd5, 0xe6, 0xf7,
        0x80, 0x91, 0xa2, 0xb3, 0xc4, 0xd5, 0xe6, 0xf7,
        't', 'e', 's', 't', 'm', 'e', 0, 0, 0, 't', 'e', 's', 't', 'm', 'e', 'f', 'u', 'r'};
    static_assert(sizeof(mb) == sizeof(mb_expected));
    dynamic_assert(memcmp(&mb, mb_expected, sizeof(mb_expected)) == 0);

    constexpr test_message<LittleEndian> ml;
    uint8_t ml_expected[] = {
        0xAF, 0x91, 0x80, 0x91, 0x80,
        0xf7, 0xe6, 0xd5, 0xc4, 0xb3, 0xa2, 0x91, 0x80,
        0xf7, 0xe6, 0xd5, 0xc4, 0xb3, 0xa2, 0x91, 0x80,
        't', 'e', 's', 't', 'm', 'e', 0, 0, 0, 't', 'e', 's', 't', 'm', 'e', 'f', 'u', 'r' };
    static_assert(sizeof(ml) == sizeof(ml_expected));
    dynamic_assert(memcmp(&ml, ml_expected, sizeof(ml_expected)) == 0);
}

void tests()
{
    static_assert(0x3412 == simple_byteswap<uint16_t>(0x1234));

    static_assert(std::is_same_v<decltype(ArrayChar<5>().to_string_view())::value_type, char>);
    static_assert(std::is_same_v<decltype(ArrayChar8<5>().to_string_view())::value_type, char8_t>);
    test_array<char>::test();
    test_array<char8_t>::test();

    test_type<char8_t>();
    test_type<int8_t>();
    test_type<uint8_t>();
    test_type<int16_t>();
    test_type<uint16_t>();
    test_type<int32_t>();
    test_type<uint32_t>();
    test_type<int64_t>();
    test_type<uint64_t>();
    test_type<float>();
    test_type<double>();

    test_messages();
}

}  // namespace openmsg

int main(int argc, char* argv[])
{
    (void)argc;
    (void)argv;
    openmsg::tests();
}
