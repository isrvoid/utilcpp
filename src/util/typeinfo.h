#pragma once

#include <util/digest/crc.h>

namespace util {

struct TypeInfo {
    uint32_t hash;
    uint32_t size;
    const void* serde;
};

template<typename T>
class TypeName {
private:
    struct NamePtr {
        static constexpr const char* _getPtr() {
            const char* p = __PRETTY_FUNCTION__;
            while (*p++ != '=') { }
            return p + 1;
        }

        static constexpr const char* p = _getPtr();
    };

    static constexpr size_t _length() {
        const char* p = NamePtr::p;
        size_t length = 0;
        for (; !(*p == '\0' || *p == ']'); p++) {
            if (*p == ' ' && p[-1] == '>')
                continue;

            ++length;
        }
        return length;
    }

public:
    static constexpr size_t length = _length();

private:
    template<size_t length>
    struct Name {
        char a[length + 1]{};
    };

    static constexpr Name<length> _name() {
        Name<length> name;
        char* dest = name.a;
        const char* p = NamePtr::p;
        for (; !(*p == '\0' || *p == ']'); p++) {
            if (*p == ' ' && p[-1] == '>')
                continue;

            *dest++ = *p;
        }
        return name;
    }

    static constexpr Name<length> _nameStore = _name();

public:
    static constexpr const char* name = _nameStore.a;
};

template<typename T>
struct CtTypeInfo {
    static constexpr unsigned int hash() noexcept {
        util::digest::CRC32 crc;
        const char* p = name();
        while (*p != '\0')
            crc.put(*p++);

        return crc.finish();
    }

    static constexpr const char* name() noexcept {
        return TypeName<T>::name;
    }
};

// convenience functions
template<typename T>
constexpr uint32_t typeHash() noexcept {
    return CtTypeInfo<T>::hash();
}

template<typename T>
constexpr uint32_t typeHash(const T&) noexcept {
    return typeHash<T>();
}

template<typename T>
constexpr const char* typeName() noexcept {
    return TypeName<T>::name;
}

template<typename T>
constexpr const char* typeName(const T&) noexcept {
    return TypeName<T>::name;
}

} // namespace util

