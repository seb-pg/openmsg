// openmsg by Sebastien Rubens
//
// To the extent possible under law, the person who associated CC0 with
// openmsg has waived all copyright and related or neighboring rights
// to openmsg.
//
// You should have received a copy of the CC0 legalcode along with this
// work.  If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.

#include "openmsg/endian_wrapper.h"  // This is the bit to include when using BigEndian or LittleEndian wrappers
#include "openmsg/array_char.h"      // This is the bit to include when using ArrayChar or ArrayChar8
#include <iostream>

namespace openmsg {

#pragma pack(push)
#pragma pack(1)

template<template<typename...> class _W = BigEndian>
struct example_message
{                                                   // serialised values:
    _W<uint32_t> a = 0xDEADBEEFu;                   // deadbeef
    _W<Optionull<uint16_t>> b[2] = { 0xDEAD };      // dead, ffff
    _W<Optionull<uint32_t>> c[2] = { 0xDEADBEEF };  // deadbeef, ffffffff
    ArrayChar<9> d = "testme";                      // 746573746d65000000
    ArrayChar<9> e = "testmefurther";               // 746573746d65667572
};

#pragma pack(pop)

struct test_message2
{
    LittleEndian<uint32_t> a = 0xDEADBEEFu;
};

void example()
{
    std::cout << std::hex;
    constexpr char values[] = "0123456789abcdef";

    constexpr example_message m;
    auto p = reinterpret_cast<const unsigned char*>(&m);
    for (size_t i = 0; i != sizeof(m); ++i)
        std::cout << values[p[i] >> 4] << values[p[i] & 0xF];
    // deadbeefdeadffffdeadbeefffffffff746573746d65000000746573746d65667572
    std::cout << std::endl;

    constexpr test_message2 m2;
    p = reinterpret_cast<const unsigned char*>(&m2);
    for (size_t i = 0; i != sizeof(m2); ++i)
        std::cout << values[p[i] >> 4] << values[p[i] & 0xF];
    // efbeadde
    std::cout << std::endl;
}

}  // namespace openmsg
