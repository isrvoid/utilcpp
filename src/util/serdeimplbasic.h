#pragma once

#include <cstdint>
#include <cstddef>

namespace util {
namespace serde {

constexpr inline void writeUint(size_t length, void*& p) {
    *static_cast<uint32_t*>(p) = static_cast<uint32_t>(length);
    p = static_cast<uint8_t*>(p) + 4;
}

constexpr inline void readUint(const void*& p, uint32_t& length) {
    length = *static_cast<const uint32_t*>(p);
    p = static_cast<const uint8_t*>(p) + 4;
}

void* serializestring(const void* p, void* dest, const void* destEnd) noexcept;
const void* deserializestring(const void* src, const void* srcEnd, void* ptrOut) noexcept;

} // namespace serde
} // namespace util
