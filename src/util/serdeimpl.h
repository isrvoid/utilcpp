#pragma once

#include <memory>
#include <type_traits>
#include <vector>

// serdespecbasic.h is not included to allow extensions

namespace util {
namespace serde {

// TODO use template for list type; remove sfinae - simple forwarding (bool template arg)
template<typename T>
auto serializevector(const void* _p, void* dest, const void* destEnd)
    noexcept -> std::enable_if_t<isTrivialSerde<T>(), void*> {
	auto p = static_cast<const std::vector<T>*>(_p);
	const size_t length = p->size();
    const size_t size = length * sizeof(T);
	if (static_cast<uint8_t*>(dest) + 4 + size > destEnd)
		return nullptr;

    writeUint(length, dest);
	memcpy(dest, p->data(), size);
	return static_cast<uint8_t*>(dest) + size;
}

template<typename T>
auto deserializevector(const void* src, const void* srcEnd, void* ptrOut)
    noexcept -> std::enable_if_t<isTrivialSerde<T>(), const void*> {
	if (static_cast<const uint8_t*>(src) + 4 > srcEnd)
		return nullptr;

	uint32_t length;
    readUint(src, length);
    const size_t size = length * sizeof(T);
	if (static_cast<const uint8_t*>(src) + size > srcEnd)
		return nullptr;

	auto p = std::make_shared<std::vector<T>>(length);
	memcpy(p->data(), src, size);
	*static_cast<decltype(p)*>(ptrOut) = p;
	return static_cast<const uint8_t*>(src) + size;
}

template<typename T>
auto serializevector(const void* _p, void* dest, const void* destEnd)
    noexcept -> std::enable_if_t<!isTrivialSerde<T>(), void*> {
	if (static_cast<uint8_t*>(dest) + 4 > destEnd)
		return nullptr;

	auto p = static_cast<const std::vector<T>*>(_p);
    writeUint(p->size(), dest);
    auto serialize = serdePair<T>().first;
    for (const auto& e : *p)
        if (!(dest = serialize(&e, dest, destEnd)))
            return nullptr;

    return dest;
}

template<typename T>
auto deserializevector(const void* src, const void* srcEnd, void* ptrOut)
    noexcept -> std::enable_if_t<!isTrivialSerde<T>(), const void*> {
	if (static_cast<const uint8_t*>(src) + 4 > srcEnd)
		return nullptr;

	uint32_t length;
    readUint(src, length);
	auto p = std::make_shared<std::vector<T>>(length);
    auto deserialize = serdePair<T>().second;
    for (size_t i = 0; i < length; ++i) {
        std::shared_ptr<T> e;
        if (!(src = deserialize(src, srcEnd, &e)))
            return nullptr;

        (*p)[i] = std::move(*e);
    }

	*static_cast<decltype(p)*>(ptrOut) = p;
    return src;
}

} // namespace serde
} // namespace util
