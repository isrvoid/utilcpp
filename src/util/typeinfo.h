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

    // TODO name(), resuse name in hash()
    static constexpr unsigned int hash() {
        util::digest::CRC32 crc;
        // __PRETTY_FUNCTION__:
        // static constexpr unsigned int util::TypeInfoFactory<T>::hash() [with T = unsigned char]
        constexpr const char* func = __PRETTY_FUNCTION__;
        constexpr size_t squareBracketIndex = 63;
        static_assert('[' == func[squareBracketIndex], "Wrong '[' index (function signature changed?)");
        const char* p = func + squareBracketIndex + 9; // "9: with T = ".length();
        while (*++p != ']') {
            // skip spaces in closing '>' (e.g. std::shared_ptr<std::vector<int> >)
            if (*p == ' ' && p[-1] == '>')
                ++p;

            crc.put(*p);
        }

        return crc.finish();
    }
};

template<typename T>
constexpr uint32_t typeHash(const T&) {
    static_assert(!std::is_pointer<T>::value, "Don't pass pointers (const T* and T* differ)");
    return TypeInfoFactory<T>::hash();
}

// TODO typeName()

} // namespace util

