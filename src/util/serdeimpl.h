#pragma once

#include <cstdint>
#include <cstddef>
#include <cstring>
#include <memory>
#include <vector>

namespace util {
namespace serde {

constexpr inline void writeUint(void*& p, size_t length) {
    *static_cast<uint32_t*>(p) = static_cast<uint32_t>(length);
    p = static_cast<uint8_t*>(p) + 4;
}

constexpr inline void readUint(const void*& p, uint32_t& length) {
    length = *static_cast<const uint32_t*>(p);
    p = static_cast<const uint8_t*>(p) + 4;
}

template<typename T>
void* serializevector(const void* _v, void* dest, const void* destEnd) noexcept {
	auto v = static_cast<const std::vector<T>*>(_v);
	const size_t length = v->size() * sizeof(T);
	if (static_cast<uint8_t*>(dest) + 4 + length > destEnd)
		return nullptr;

    writeUint(dest, length);
	memcpy(dest, v->data(), length);
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

	auto v = std::make_shared<std::vector<T>>(length / sizeof(T));
	memcpy(v->data(), src, length);
	*static_cast<decltype(v)*>(ptrOut) = v;
	return static_cast<const uint8_t*>(src) + length;
}

} // namespace serde
} // namespace util
