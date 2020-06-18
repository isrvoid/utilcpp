#pragma once

#include <cstdint>
#include <cassert>

#include <util/digest/crc.h>

template<typename T>
struct _AspectIdLut;

namespace util {

template<typename T>
constexpr uint32_t aspectId(T enumerator) noexcept {
    assert(enumerator != T::_end);
    return ::_AspectIdLut<T>::id(enumerator);
}

constexpr uint32_t addIndex(uint32_t id, size_t index) noexcept {
    // TODO relax assert; put() 1 to 4 bytes depending on index
    assert(index < 1 << 8);
    digest::CRC32 crc{id};
    crc.put(static_cast<uint8_t>(index));
    return crc.finish();
}

constexpr uint32_t maybeAddIndex(uint32_t id, size_t index, bool shouldAdd) noexcept {
    if (!shouldAdd)
        return id;

    return addIndex(id, index);
}

} // namespace util
