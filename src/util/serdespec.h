#pragma once

#include <cstdint>
#include <utility>
#include <string>
#include <vector>

#include <util/typeinfo.h>
#include <util/serdeifc.h>
#include <util/serdeimpl.h>

#define _SERDE_SPEC(TYPE, SER, DE)  template<> struct _SerdeSpec<TypeHash<TYPE>::hash> { \
    const SerdePair value{SER, DE}; };

#define SERDE_SPEC(NS, TYPE)  _SERDE_SPEC(NS::TYPE, serialize ## TYPE, deserialize ## TYPE)

namespace util {
namespace serde {

using SerdePair = std::pair<serializeFun, deserializeFun>;

template<uint32_t>
struct _SerdeSpec {
    const SerdePair value{nullptr, nullptr};
};

template<typename T>
constexpr SerdePair serdePair() noexcept {
    return _SerdeSpec<TypeHash<T>::hash>{}.value;
}

SERDE_SPEC(std, vector<uint8_t>)

} // namespace serde
} // namespace util
