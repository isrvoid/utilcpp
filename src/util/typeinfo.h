#pragma once

#include <type_traits>
#include <util/digest/crc.h>

namespace util {

struct TypeInfo {
    uint32_t hash;
    uint32_t size;
    const void* serde;
};

template<typename T>
struct TypeInfoFactory {
    static TypeInfo create() {
        static const TypeInfo ti{hash(), sizeof(T)}; // FIXME serde
        return ti;
    }

    static constexpr unsigned int hash() {
        util::digest::CRC32 crc;
        const char* p = name().name - 1;
        while (*++p != '\0')
            crc.put(*p);

        return crc.finish();
    }

private:
    struct NamePtr {
        const char* p;
        size_t length;
    };

    static constexpr NamePtr namePtr() {
        // __PRETTY_FUNCTION__ example:
        // static constexpr util::TypeInfoFactory<T>::NamePtr util::TypeInfoFactory<T>::namePtr() [with T = int]
        constexpr const char* func = __PRETTY_FUNCTION__;
        constexpr size_t squareBracketIndex = 87;
        static_assert('[' == func[squareBracketIndex], "Wrong '[' index (function signature changed?)");
        constexpr const char* start = func + squareBracketIndex + 10; // 10: 1 + "with T = ".length();
        const char* p = start - 1;
        size_t closingGtCount = 0;
        while (*++p != ']')
            if (*p == '>')
                ++closingGtCount;

        const size_t length = closingGtCount < 2 ? p - start : p - start + 1 - closingGtCount;
        return NamePtr{start, length};
    }

    template<size_t length>
    struct Name {
        char name[length + 1]{};
    };

public:
    static constexpr auto name() {
        constexpr auto ptr = namePtr();
        Name<ptr.length> name;
        const char* pSrc = ptr.p - 1;
        char* pDest = name.name - 1;
        while (*++pSrc != ']') {
            if (*pSrc == ' ' && pSrc[-1] == '>')
                ++pSrc;

            *++pDest = *pSrc;
        }

        name.name[ptr.length] = '\0';
        return name;
    }
};

template<typename T>
constexpr uint32_t typeHash(const T&) {
    static_assert(!std::is_pointer<T>::value, "Don't pass pointers (const T* and T* differ)");
    return TypeInfoFactory<T>::hash();
}

template<typename T>
const char* typeName(const T&) {
    static auto name = TypeInfoFactory<T>::name();
    return name.name;
}

} // namespace util

