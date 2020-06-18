#include "lengthencoding.h"

#include <cstdint>
#include <cassert>

namespace util {

// to ensure compatibility between versions
// length should be stored in big-endian format (network byte order)

const void* LengthEncoding::read(const void* _src, size_t& lengthOut) noexcept {
    auto src = static_cast<const uint8_t*>(_src);
    const unsigned int byteCount = 1 << (*src >> 6);
    uint64_t length = 0;
    uint8_t* p = reinterpret_cast<uint8_t*>(&length);

    switch (byteCount) {
        case 8: *p++ = src[7];
                *p++ = src[6];
                *p++ = src[5];
                *p++ = src[4];
                [[fallthrough]];
        case 4: *p++ = src[3]; // this case is never encoded, but can be decoded
                *p++ = src[2];
                [[fallthrough]];
        case 2: *p++ = src[1];
                [[fallthrough]];
        case 1: *p = src[0] & 0x3f;
    }
    lengthOut = static_cast<size_t>(length);
    return src + byteCount;
}

void* LengthEncoding::write(size_t _length, void* _dest) noexcept {
    assert(_length <= lengthMax);
    auto dest = static_cast<uint8_t*>(_dest);
    const unsigned int byteCountLog = (_length > shortLengthMax) << 1 | (_length > byteLengthMax);
    const unsigned int byteCount = 1 << byteCountLog;
    uint64_t length = _length;
    uint8_t* p = reinterpret_cast<uint8_t*>(&length);
    switch (byteCount) {
        case 8: dest[7] = *p++;
                dest[6] = *p++;
                dest[5] = *p++;
                dest[4] = *p++;
                dest[3] = *p++;
                dest[2] = *p++;
                [[fallthrough]];
        case 2: dest[1] = *p++;
                [[fallthrough]];
        case 1: dest[0] = static_cast<uint8_t>(*p | byteCountLog << 6);
    }

    return dest + byteCount;
}

void* LengthEncoding::writeBack(size_t _length, void* _dest) noexcept {
    assert(_length <= lengthMax);
    auto dest = static_cast<uint8_t*>(_dest);
    const unsigned int byteCountLog = (_length > shortLengthMax) << 1 | (_length > byteLengthMax);
    uint64_t length = _length;
    uint8_t* p = reinterpret_cast<uint8_t*>(&length);
    switch (1 << byteCountLog) {
        case 8: *--dest = *p++;
                *--dest = *p++;
                *--dest = *p++;
                *--dest = *p++;
                *--dest = *p++;
                *--dest = *p++;
                [[fallthrough]];
        case 2: *--dest = *p++;
                [[fallthrough]];
        case 1: *--dest = static_cast<uint8_t>(*p | byteCountLog << 6);
    }
    return dest;
}

} // namespace util
