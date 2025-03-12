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
#include "openmsg/endian_wrapper.hpp"
#include "openmsg/memory_wrapper.hpp"
#include "openmsg/optionull.hpp"

#include "inttypes.h"

#include <cassert>
#include <format>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <source_location>
#include <string.h>
#include <vector>

namespace openmsg {

namespace constants {

constexpr uint16_t w2 = 0x8091;
constexpr uint32_t w4 = 0x8091a2b3;
constexpr uint64_t w8 = 0x8091a2b3c4d5e6f7ull;

constexpr uint16_t r2 = 0x9180;
constexpr uint32_t r4 = 0xb3a29180;
constexpr uint64_t r8 = 0xf7e6d5c4b3a29180;

}  // namespace constants

void __dynamic_assert(bool value, std::source_location loc)
{
    if (!bool(value))
        throw std::runtime_error(std::format("error in {} at line {}", loc.file_name(),  loc.line()));
}

#define dynamic_assert(value) __dynamic_assert((value), std::source_location::current())

void test_array_char()
{
    // default ctor
    constexpr ArrayChar<5> a;  // filled with zeros by default
    static_assert(a.to_string_view(false).length() == a.size);
    static_assert(a.to_string_view(true).empty());
    static_assert(a().empty());
    static_assert(a.empty());
    constexpr ArrayChar<5> b('x');  // filled with 'x' by default
    static_assert(b.to_string_view(true).length() == b.size);

    // ctor from char[]
    constexpr ArrayChar<8> b1("testmefurther");  // non zero terminated
    constexpr ArrayChar<8, true> b2("testmefurther");  // zero terminated
    constexpr ArrayChar<8> b3(b2);
    static_assert(b1 != b2);
    static_assert(b2 == b3);
    static_assert(b1.to_string_view(false).length() == b1.length());
    static_assert(b2.to_string_view(true).length() == b2.length());
    static_assert(b2().length() == b2.length());

    // "copy" ctor
    constexpr ArrayChar<5> c1(b2);
    constexpr ArrayChar<15> c2(b2);
    constexpr ArrayChar<8, true> c3(b2);
    static_assert(c1.length() == c1.size);
    static_assert(c2.length() == b2.length());
    static_assert(c3.length() == b2.length());

    // operator=
    ArrayChar<5> d1;
    ArrayChar<5> d2(c1);
    d1 = c1;
    dynamic_assert(d1 == c1);
    ArrayChar<15> d3;
    ArrayChar<15> d4("testmefurther");
    d3 = "testmefurther";
    dynamic_assert(d3 == d4);

    // misc
    ArrayChar<5> e1('x');
    ArrayChar<5> e2("xxxxxx");
    dynamic_assert(e1 == e2);
    e1.clear();
    dynamic_assert(e1.empty());

    // value_type of to_string_view()
    static_assert(std::is_same_v<decltype(ArrayChar<5>().to_string_view())::value_type, char>);
    static_assert(std::is_same_v<decltype(ArrayChar8<5>().to_string_view())::value_type, char8_t>);

    // misc (linked to from_array_pointer)
    std::string testmefurther = "testmefurther";
    std::string_view testmefurther_sv = testmefurther;
    ArrayChar<5> f1(testmefurther_sv);
    ArrayChar<5> f2;
    f2 = testmefurther;
    dynamic_assert(f1 == f2);
    f2.from_array_pointer(testmefurther.c_str());
    dynamic_assert(f1 == f2);
}

void test_bswap()
{
    // check if bswap() can be constexpr evaluated, and with the correct values
    constexpr auto ce2 = bswap(constants::w2);
    constexpr auto ce4 = bswap(constants::w4);
    constexpr auto ce8 = bswap(constants::w8);
    static_assert(ce2 == constants::r2);
    static_assert(ce4 == constants::r4);
    static_assert(ce8 == constants::r8);

    // check if bswap() can be NOT constexpr evaluated, and with the correct values
    auto v2 = bswap(constants::w2);
    auto v4 = bswap(constants::w4);
    auto v8 = bswap(constants::w8);
    dynamic_assert(v2 == constants::r2);
    dynamic_assert(v4 == constants::r4);
    dynamic_assert(v8 == constants::r8);
}

void test_serialisable()
{
    // check if serialisable concept is working as intended
    enum E : uint16_t {};
    struct S {};
    static_assert(serialisable<uint16_t>);
    static_assert(serialisable<double>);
    static_assert(serialisable<E>);
    static_assert(!serialisable<S>);
}

template<serialisable _T1, serialisable _T2>
struct _TestOptionull1
{
    using value_type = _T1;
    constexpr static _T2 nullValue = 0;  // "optionull" have "nullValue"
};

template<serialisable _T1, serialisable _T2>
struct _TestOptionull2
{
    using value_type = _T1;
    constexpr static _T2 null_value = 0;  // "optionull" NOT have "null_value", of the same type as _T1
};

void test_optionull()
{
    // Check if our fake optionull (above), behave as intended
    static_assert(optionullable<_TestOptionull1<uint16_t, uint16_t>>);
    static_assert(!optionullable<_TestOptionull2<uint16_t, uint16_t>>);
    static_assert(!optionullable<_TestOptionull1<uint16_t, uint32_t>>);

    // Check if optionullable<uint64_t, nullValue> is set correctly and its default value is initialised to nullValue
    constexpr uint64_t nullValue = ~0ull;
    constexpr Optionull<uint64_t, nullValue> on1;
    static_assert(on1.nullValue == nullValue);
    static_assert(on1() == nullValue);

    // Check if optionullable<uint64_t, nullValue> is set correctly and its default value is initialised to the user value (ctor)
    constexpr Optionull<uint64_t, nullValue> on2 = 1;
    static_assert(on1.nullValue == nullValue);
    static_assert(on2() == 1);

    // Check if optionullable<uint64_t> is set correctly and its default value is initialised to the correct "OptionullDefault"
    constexpr Optionull<int64_t> on3;
    static_assert(on3.nullValue == OptionullDefault<int64_t>::null);
    constexpr Optionull<uint64_t> on4;
    static_assert(on4.nullValue == OptionullDefault<uint64_t>::null);

    // Check floating point values
#if !defined(__clang__)  // error from clang 12: "sorry, non-type template argument of type 'double' is not yet supported"
    constexpr Optionull<float> on5;
    constexpr Optionull<double> on6;
    constexpr auto nan32 = std::bit_cast<uint32_t>(std::numeric_limits<float>::quiet_NaN());
    constexpr auto nan64 = std::bit_cast<uint64_t>(std::numeric_limits<double>::quiet_NaN());
    static_assert(std::bit_cast<uint32_t>(on5.nullValue) == nan32);
    static_assert(std::bit_cast<uint64_t>(on6.nullValue) == nan64);
#endif
}

enum class E : uint16_t { e = constants::w2, nullValue = 65535 };
struct B { uint32_t b1 : 3; uint32_t b2 : 2; int32_t b3 : 3; };

template<template<typename...> class _memory_wrapper, std::endian _endian, typename _M, typename _H>
void _test_memory_wrapper2(const _H& x)
{
    using memory_wrapper = _memory_wrapper<_H, _M, std::integral_constant<std::endian, _endian>>;
    _M memory_value = memory_wrapper::htom(x);
    auto host_value = memory_wrapper::mtoh(memory_value);
    dynamic_assert(host_value == x);
}

template<template<typename...> class _memory_wrapper, std::endian _endian, typename _T, typename ... _Args>
void _test_endian_wrapper2(_Args&&... args)
{
    using OurEndian = EndianWrapper<_T, _memory_wrapper, _endian>;
    using value_type = typename OurEndian::value_type;
    using storage_type = typename OurEndian::storage_type;

    //std::cout << typeid(OurEndian).name();
    //std::cout << "(" << (_endian == std::endian::big ? "big" : "little") << " endian";
    //std::cout << ", " << typeid(value_type).name() << " -> " << typeid(storage_type).name();
    //std::cout << ")" << std::endl;

    constexpr char unset_value = static_cast<char>(0xCCu);

    // the following should always compile, and was added to replace because the constexpr-ness is lost in args
    if constexpr (sizeof...(_Args) == 0)
        constexpr OurEndian oe;
    else if constexpr (optionullable<OurEndian>)
        constexpr OurEndian oe = _T::nullValue;
    else
        constexpr OurEndian oe = static_cast<_T>(0);

    std::vector<char> unset_buf(512, unset_value);
    std::vector<char> buf(512, unset_value);
    auto obj = *new (&*buf.begin()) OurEndian(std::forward<_Args>(args)...);

    auto host_value = obj();

    value_type casted_host_value = obj;
    static_assert(std::is_same_v<decltype(casted_host_value), decltype(host_value)>);
    dynamic_assert(casted_host_value == host_value);

    auto stored_value = obj.storage_value();
    static_assert(sizeof(obj) == sizeof(host_value));
    static_assert(sizeof(obj) == sizeof(stored_value));
    static_assert(std::is_same_v<value_type, decltype(host_value)>);
    static_assert(std::is_same_v<storage_type, decltype(stored_value)>);

    if constexpr (!optionullable<OurEndian> && sizeof...(_Args) == 0)
        dynamic_assert(buf == unset_buf);

    if constexpr (_endian == std::endian::native)
        dynamic_assert(stored_value == std::bit_cast<storage_type>(host_value));
    else
    {
        auto bswap_host_value = detail_constexpr::bswap(std::bit_cast<as_uint_type_t<value_type>>(host_value));
        dynamic_assert(stored_value == std::bit_cast<storage_type>(bswap_host_value));
    }
}

template<template<typename...> class _memory_wrapper, std::endian _endian>
void _test_endian_wrapper1()
{
    // test if the memory wrapper behaves as intended
    _test_memory_wrapper2<_memory_wrapper, _endian, uint64_t, double>(1.0);
    _test_memory_wrapper2<_memory_wrapper, _endian, uint16_t, E>(E::e);

    // test endian wrapper on "direct" types
    _test_endian_wrapper2<_memory_wrapper, _endian, uint32_t>();
    _test_endian_wrapper2<_memory_wrapper, _endian, uint32_t>(constants::w4);
    _test_endian_wrapper2<_memory_wrapper, _endian, E>();
    _test_endian_wrapper2<_memory_wrapper, _endian, E>(E::e);
    _test_endian_wrapper2<_memory_wrapper, _endian, double>();
    _test_endian_wrapper2<_memory_wrapper, _endian, double>(1.0);

    // test endian wrapper on "Optionull" types (same as direct types, but wrapped)
    _test_endian_wrapper2<_memory_wrapper, _endian, Optionull<uint32_t, ~1u>>();
    _test_endian_wrapper2<_memory_wrapper, _endian, Optionull<uint32_t, ~1u>>(5u);
    _test_endian_wrapper2<_memory_wrapper, _endian, Optionull<E, E::nullValue>>();
    _test_endian_wrapper2<_memory_wrapper, _endian, Optionull<E, E::nullValue>>(E::e);
#if !defined(__clang__)  // error from clang 12: "sorry, non-type template argument of type 'double' is not yet supported"
    _test_endian_wrapper2<_memory_wrapper, _endian, Optionull<double, -1.0>>();
    _test_endian_wrapper2<_memory_wrapper, _endian, Optionull<double, -1.0>>(1.0);
#endif
}

void test_endian_wrappers()
{
    _test_endian_wrapper1<memory_wrapper_robust, std::endian::big>();
    _test_endian_wrapper1<memory_wrapper_bswap, std::endian::big>();
    _test_endian_wrapper1<memory_wrapper_movbe, std::endian::big>();

    _test_endian_wrapper1<memory_wrapper_robust, std::endian::little>();
    _test_endian_wrapper1<memory_wrapper_bswap, std::endian::little>();
    _test_endian_wrapper1<memory_wrapper_movbe, std::endian::little>();
}

#pragma pack(push)
#pragma pack(1)

template<template<typename...> class _W>
struct test_message
{
    _W<uint8_t> a = 0xAFu;
    _W<Optionull<uint16_t>> b[2] = { constants::w2, constants::w2 };
    _W<Optionull<uint64_t>> c[2] = { constants::w8, constants::w8 };
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
    test_array_char();
    test_bswap();
    test_serialisable();
    test_optionull();
    test_endian_wrappers();
    test_messages();
}

}  // namespace openmsg

int main(int argc, char* argv[])
{
    (void)argc;
    (void)argv;
    openmsg::tests();
}
