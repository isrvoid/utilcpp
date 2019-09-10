#pragma once

#include <util/typeinfo.h>

namespace util {

template<typename T>
struct TypeInfoFactory {
    static TypeInfo create() noexcept {
        static const TypeInfo ti = ctCreate();
        return ti;
    }

    static constexpr TypeInfo ctCreate() noexcept {
        return TypeInfo{hash(), sizeof(T)}; // FIXME serde
    }
};

} // namespace util
