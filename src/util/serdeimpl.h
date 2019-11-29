#pragma once

#include <vector>

#include <util/serdeimplbasic.h>
// serdespecbasic.h is not included to allow extensions

namespace util {
namespace serde {

template<typename T>
auto serializevector(const void* p, void* dest, const void* destEnd) {
    return serializeList<std::vector<T>, serdePair<T>().first>(p, dest, destEnd);
}

template<typename T>
auto deserializevector(const void* src, const void* srcEnd, void* ptrOut) {
    return deserializeList<std::vector<T>, serdePair<T>().second>(src, srcEnd, ptrOut);
}

} // namespace serde
} // namespace util
