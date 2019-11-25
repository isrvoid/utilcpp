#pragma once

#include <cstdint>
#include <cstddef>
#include <cstring>
#include <memory>
#include <vector>
#include <string>

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

template<typename T>
void* serializevector(const void* _p, void* dest, const void* destEnd) noexcept {
	auto p = static_cast<const std::vector<T>*>(_p);
	const size_t length = p->size() * sizeof(T);
	if (static_cast<uint8_t*>(dest) + 4 + length > destEnd)
		return nullptr;

    writeUint(length, dest);
	memcpy(dest, p->data(), length);
	return static_cast<uint8_t*>(dest) + length;
}

template<typename T>
const void* deserializevector(const void* src, const void* srcEnd, void* ptrOut) noexcept {
	if (static_cast<const uint8_t*>(src) + 4 > srcEnd)
		return nullptr;

	uint32_t length;
    readUint(src, length);
	if (static_cast<const uint8_t*>(src) + length > srcEnd || length % sizeof(T))
		return nullptr;

	auto p = std::make_shared<std::vector<T>>(length / sizeof(T));
	memcpy(p->data(), src, length);
	*static_cast<decltype(p)*>(ptrOut) = p;
	return static_cast<const uint8_t*>(src) + length;
}

void* serializestring(const void* p, void* dest, const void* destEnd) noexcept;
const void* deserializestring(const void* src, const void* srcEnd, void* ptrOut) noexcept;

} // namespace serde
} // namespace util
