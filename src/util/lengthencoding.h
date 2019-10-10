#pragma once

#include <cstddef>

// this implementation currently only works on little-endian machines and is not thread safe

namespace util {

class LengthEncoding {
    LengthEncoding() = delete;

public:
    static constexpr unsigned int byteCountMax = 8;
    static constexpr unsigned int byteLengthMax = 0xff >> 2;
    static constexpr unsigned int shortLengthMax = 0xffff >> 2;
    static constexpr size_t lengthMax = static_cast<size_t>(-1) >> 2 * (sizeof(size_t) >= 8);

    static const void* read(const void* src, size_t& lengthOut) noexcept;
    static void* write(size_t length, void* dest) noexcept;
    static void* writeBack(size_t length, void* dest) noexcept;
};

} // namespace util
