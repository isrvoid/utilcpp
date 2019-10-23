#pragma once

#include <type_traits>

#define ASPECT_TYPE(ASPECT, TYPE)  template<> \
    struct _AspectTypeSpec<decltype(ASPECT), ASPECT> { \
        using type = TYPE; \
    };

template<typename Aspect, Aspect val, typename = std::enable_if_t<std::is_enum<Aspect>::value>>
struct _AspectTypeSpec {
    using type = void;
};
