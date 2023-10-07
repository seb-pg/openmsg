# openmsg messaging library

Author: [Sébastien Rubens](https://www.linkedin.com/in/sebastienrubens/)

----

## Licensing (creative commons CC0)

To the extent possible under law, the person who associated CC0 with
openmsg has waived all copyright and related or neighboring rights
to openmsg.

You should have received a copy of the CC0 legalcode along with this
work.  If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.

----

## Open messaging library

*openmsg* is a generic C++20 library making binary messages (protocols)
decoding and encoding easier. An example of use would be the decoding of
IP stack protocols, communication protocols based on SBE (simple binary
encoding), etc.  While easy to write, no specification processing tool
is provided (such as processing SBE specifications to generate lightweight
messages) as I wanted to keep this utility simple and "focused".

*openmsg* provides low-level wrappers to deal with endianess and other
things like "optionull" values.

*openmsg* can be used to decode multiple binary protocol such as
TCP/IP headers, Simple Binary Encoding (aka SBE) messages, etc.

*openmsg* has been tested with:
* clang version 12.0.0
* g++ version 11.1.0
* msvc: 14.29 (_MSC_VER 1929)

*openmsg* does not make choices with regards to
* cpu abilities,
* memory aliasing, 
* memory access reordering issues, either software or hardware, such as
compiler memory barrier or things like read-after-write hazard (other
variants). 

These choices are left to the user who is responsible for defining the
appropriate endian_wrapper_user (in include/openmsg/user_definitions.h).

Some memory_wrapper are provided in include/openmsg/memory_wrapper.h, such as
* memory_wrapper_bswap (default)
* memory_wrapper_movbe
* memory_wrapper_robust

*openmsg* does not provide a parser for messages specifications (such as
SBE xml files): it is only the low-level layer.

----

## Files description

<u>src/example.cpp</u>

>A simple example of message and memory layout

<u>src/test.cpp</u>

>A set of tests to check the library works as expected.

<u>include/openmsg/array_char.h</u>

>A fixed size array-of-1byte-character wrapper.

<u>include/openmsg/bswap.h</u>

>A <bit>-like function for bswap, which makes full use of
>`std::is_constant_evaluated()`.

<u>include/openmsg/endian_wrapper.h</u>

>This is the main wrapper to deal with near-seamless endianess.

>A lot of things are done are compiling time and if the message
>has the same endianess and the host, not byte swapping is done,
>and the user\'s debugger should show the native values.

>Similarly, if the message does not have the same endianess and
>the host, byte swapping will be done implicitly, and the user\'s
>debugger will show an appropriate type for the swapped value.

>Things like this are possible as constexpr:

>>`struct test_message2 { LittleEndian<uint32_t> a = 0xDEADBEEFu; };`

>>`constexpr test_message2 m2;`

>test_message2 will serialise as "efbeadde" (little endian) or "deadbeef"
>(big endian).

<u>include/openmsg/memory_wrapper.h</u>

>3 ready-to-use memory wrappers are provided.
>These wrappers offer 2 functions:
>- mtoh(): message to host, would be similar to network to host, aka ntoh()
>- htom(): host to message, would be similar to host to network, aka hton()

<u>include/openmsg/optionull.h</u>

>This is a wrapper to deal with "SBE" nullValue.
>It wraps some data and store a constexpr value to identify the value
>considered as null.
>It extends on SBE by providing nullValue for float and double (quiet nan).
    
<u>include/openmsg/user_definitions.h</u>

>User defined value for endian_wrapper_user.

----

## Notes

The library is provided "as is". While the author is happy to provide reasonable
assistance, there is no guarantee any assistance will be provided.
