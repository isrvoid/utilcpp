#pragma once

#include <util/typeinfo.h>
#include <util/serdespec.h>

namespace util {

template<typename T>
struct TypeInfoFactory {
    static const TypeInfo& create() noexcept {
        static const TypeInfo ti = ctCreate();
        return ti;
    }

    static constexpr TypeInfo ctCreate() noexcept {
        const auto& serde = serde::Spec<TypeHash<T>::hash>::value;
        return TypeInfo{TypeHash<T>::hash, sizeof(T), serde.first ? &serde : nullptr};
    }
};

} // namespace util
