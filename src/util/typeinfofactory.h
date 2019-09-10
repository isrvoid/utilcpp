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
        const auto serde = reinterpret_cast<const void* const*>(&serde::Spec<CtTypeInfo<T>::hash()>::value);
        return TypeInfo{CtTypeInfo<T>::hash(), sizeof(T), *serde == nullptr ? nullptr : serde};
    }
};

} // namespace util
