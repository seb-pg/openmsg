# openmsg messaging library

Author: [Sébastien Rubens](https://www.linkedin.com/in/sebastienrubens/)

----

## Licensing (creative commons CC0)

To the extent possible under law, the person who associated CC0 with
openmsg has waived all copyright and related or neighboring rights
to openmsg.

You should have received a copy of the CC0 legalcode along with this
work.  If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.

The author has not registered any patent or trademark about the work provided, which was written from scratch, without any inspiration from any other works.

----

## Open messaging library

*openmsg* is a generic C++20 library making binary messages (protocols)
decoding and encoding easier. An example of use would be the decoding of
IP stack protocols, communication protocols based on Simple Binary
Encoding (SBE), etc.

*openmsg* provides low-level wrappers to deal with endianess and other
things like "Optionull" values.

*openmsg* can be used to decode multiple binary protocol such as
TCP/IP headers, Simple Binary Encoding (SBE) messages, etc.

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
appropriate endian_wrapper_user (in include/openmsg/user_definitions.hpp).

Some memory_wrapper are provided in include/openmsg/memory_wrapper.hpp, such as
* memory_wrapper_bswap (default)
* memory_wrapper_movbe
* memory_wrapper_robust
Other schemes would be easy to add, such as "read 2 words, rotate, and mask"
for processing units (CPU/DSP/GPU) not supporting non-aligned memory accesses.

*openmsg* does not provide a parser for messages specifications (such as
Simple Binary Encoding XML  files). While easy to write such generator, this
library focused on the low-level layer.

----

## Files description

<details>
<summary>src/example.cpp</summary>
A simple example of message and memory layout.
</details>

<details>
<summary>src/test.cpp</summary>
A set of tests to check the library works as expected.
</details>

<details>
<summary>include/openmsg/array_char.hpp</summary>
A fixed size array-of-1byte-character wrapper.
</details>

<details>
<summary>include/openmsg/bswap.hpp</summary>
A bit-like function for bswap, which makes full use of std::is_constant_evaluated().
</details>

<details>
<summary>include/openmsg/endian_wrapper.hpp</summary>
This is the main wrapper to deal with near-seamless endianess.

A lot of things are done are compiling time and if the message has the same endianess and
the host, not byte swapping is done, and the user's debugger should show the native values.

Similarly, if the message does not have the same endianess and the host, byte swapping will
be done implicitly, and the user's debugger will show an appropriate type for the swapped value.

Things like this are possible as constexpr:
<pre>struct test_message2 { LittleEndian<uint32_t> a = 0xDEADBEEFu; };
constexpr test_message2 m2;</pre>

test_message2 will serialise as "efbeadde" (little endian) or "deadbeef" (big endian).
</details>

<details>
<summary>include/openmsg/memory_wrapper.h</summary>
3 ready-to-use memory wrappers are provided.

These wrappers offer 2 functions:
- mtoh(): message to host, would be similar to network to host, aka ntoh()
- htom(): host to message, would be similar to host to network, aka hton()
</details>

<details>
<summary>include/openmsg/optionull.hpp</summary>
This is a wrapper to deal with Simple Binary Encoding (SBE) nullValue.

It wraps some data and store a constexpr value to identify the value
considered as null.

It extends on Simple Binary Encoding (SBE)  by providing nullValue for
float and double (quiet nan).
</details>

<details>
<summary>include/openmsg/user_definitions.hpp</summary>
User defined value for endian_wrapper_user.
</details>

----

## Notes

The library is provided "as is". While the author is happy to provide reasonable
assistance, there is no guarantee any assistance will be provided.


## C++ version

The version is provided for C++20.

It can be compiled using VisualStudio (.sln not provided) or using [CMake](https://cmake.org/cmake/help/latest/manual/cmake.1.html) (e.g. on Linux or WSL) of one which is deemed to be installed and "understood".

First, generate a project buildsystem for a specific build type (here Release, but could be Debug or RelWithDebInfo)
    cmake . -Bbuild/Release -DCMAKE_BUILD_TYPE=Release

Second, build the project
    cmake --build build/Release

Third, simply run
    ./build/Release/bin/example
    ./build/Release/bin/tests

