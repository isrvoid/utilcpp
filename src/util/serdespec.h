#pragma once

#include <cstdint>
#include <utility>
#include <string>
#include <vector>

#include <util/serdeifc.h>
#include <util/serdeimpl.h>

#define _SERDE_SPEC(TYPE, SER, DE)  struct _SerdeSpec<TYPE> { const SerdePair value{SER, DE}; };

#define SERDE_SPEC(NS, TYPE)  template<> _SERDE_SPEC(NS::TYPE, serialize ## TYPE, deserialize ## TYPE)
#define SERDE_SPEC_T(NS, TYPE)  template<typename T> _SERDE_SPEC(NS::TYPE, serialize ## TYPE, deserialize ## TYPE)

namespace util {
namespace serde {

using SerdePair = std::pair<serializeFun, deserializeFun>;

template<typename T>
struct _SerdeSpec {
    const SerdePair value{nullptr, nullptr};
};

template<typename T>
constexpr SerdePair serdePair() noexcept {
    return _SerdeSpec<T>{}.value;
}

SERDE_SPEC(std, string)
SERDE_SPEC_T(std, vector<T>)

} // namespace serde
} // namespace util
