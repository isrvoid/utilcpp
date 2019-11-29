#pragma once

#include <cstdint>
#include <cstddef>
#include <memory>
#include <type_traits>

#include <util/serdeifc.h>

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

template<typename List, serializeFun serialize, typename T = typename List::value_type>
auto serializeList(const void* p, void* dest, const void* destEnd) noexcept ->
std::enable_if_t<serialize == nullptr, void*> {
    auto list = static_cast<const List*>(p);
    const size_t length = list->size();
    const size_t size = length * sizeof(T);
    if (static_cast<uint8_t*>(dest) + 4 + size > destEnd)
        return nullptr;

    writeUint(length, dest);
    memcpy(dest, list->data(), size);
    return static_cast<uint8_t*>(dest) + size;
}

template<typename List, serializeFun serialize, typename T = typename List::value_type>
auto serializeList(const void* p, void* dest, const void* destEnd) noexcept ->
std::enable_if_t<serialize != nullptr, void*> {
    if (static_cast<uint8_t*>(dest) + 4 > destEnd)
        return nullptr;

    auto list = static_cast<const List*>(p);
    writeUint(list->size(), dest);
    for (const auto& e : *list)
        if (!(dest = serialize(&e, dest, destEnd)))
            return nullptr;

    return dest;
}

template<typename List, deserializeFun deserialize, typename T = typename List::value_type>
auto deserializeList(const void* src, const void* srcEnd, void* ptrOut) noexcept ->
std::enable_if_t<deserialize == nullptr, const void*> {
    if (static_cast<const uint8_t*>(src) + 4 > srcEnd)
        return nullptr;

    uint32_t length;
    readUint(src, length);
    const size_t size = length * sizeof(T);
    if (static_cast<const uint8_t*>(src) + size > srcEnd)
        return nullptr;

    auto list = std::make_shared<List>(length);
    memcpy(list->data(), src, size);
    *static_cast<std::shared_ptr<List>*>(ptrOut) = list;
    return static_cast<const uint8_t*>(src) + size;
}

template<typename List, deserializeFun deserialize, typename T = typename List::value_type>
auto deserializeList(const void* src, const void* srcEnd, void* ptrOut) noexcept ->
std::enable_if_t<deserialize != nullptr, const void*> {
    if (static_cast<const uint8_t*>(src) + 4 > srcEnd)
        return nullptr;

    uint32_t length;
    readUint(src, length);
    auto list = std::make_shared<List>(length);
    for (size_t i = 0; i < length; ++i) {
        std::shared_ptr<T> e;
        if (!(src = deserialize(src, srcEnd, &e)))
            return nullptr;

        (*list)[i] = std::move(*e);
    }

    *static_cast<std::shared_ptr<List>*>(ptrOut) = list;
    return src;
}

} // namespace serde
} // namespace util
