#pragma once

#include <memory>
#include <type_traits>
#include <vector>

// serdespecbasic.h is not included to allow extensions

namespace util {
namespace serde {

struct ListSerde {
    template<typename List, typename T = typename List::value_type>
    static auto serialize(const void* _p, void* dest, const void* destEnd)
        noexcept -> std::enable_if_t<isTrivialSerde<T>(), void*> {
        auto list = static_cast<const List*>(_p);
        const size_t length = list->size();
        const size_t size = length * sizeof(T);
        if (static_cast<uint8_t*>(dest) + 4 + size > destEnd)
            return nullptr;

        writeUint(length, dest);
        memcpy(dest, list->data(), size);
        return static_cast<uint8_t*>(dest) + size;
    }

    template<typename List, typename T = typename List::value_type>
    static auto serialize(const void* _p, void* dest, const void* destEnd)
        noexcept -> std::enable_if_t<!isTrivialSerde<T>(), void*> {
        if (static_cast<uint8_t*>(dest) + 4 > destEnd)
            return nullptr;

        auto list = static_cast<const List*>(_p);
        writeUint(list->size(), dest);
        auto serialize = serdePair<T>().first;
        for (const auto& e : *list)
            if (!(dest = serialize(&e, dest, destEnd)))
                return nullptr;

        return dest;
    }

    template<typename List, typename T = typename List::value_type>
    static auto deserialize(const void* src, const void* srcEnd, void* ptrOut)
        noexcept -> std::enable_if_t<isTrivialSerde<T>(), const void*> {
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

    template<typename List, typename T = typename List::value_type>
    static auto deserialize(const void* src, const void* srcEnd, void* ptrOut)
        noexcept -> std::enable_if_t<!isTrivialSerde<T>(), const void*> {
        if (static_cast<const uint8_t*>(src) + 4 > srcEnd)
            return nullptr;

        uint32_t length;
        readUint(src, length);
        auto list = std::make_shared<List>(length);
        auto deserialize = serdePair<T>().second;
        for (size_t i = 0; i < length; ++i) {
            std::shared_ptr<T> e;
            if (!(src = deserialize(src, srcEnd, &e)))
                return nullptr;

            (*list)[i] = std::move(*e);
        }

        *static_cast<std::shared_ptr<List>*>(ptrOut) = list;
        return src;
    }
};

template<typename T>
auto serializevector(const void* _p, void* dest, const void* destEnd) {
    return ListSerde::serialize<std::vector<T>>(_p, dest, destEnd);
}

template<typename T>
auto deserializevector(const void* src, const void* srcEnd, void* ptrOut) {
    return ListSerde::deserialize<std::vector<T>>(src, srcEnd, ptrOut);
}

} // namespace serde
} // namespace util
