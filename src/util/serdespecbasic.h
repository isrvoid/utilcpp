#pragma once

#include <utility>

#include <util/serdeifc.h>
#include <util/serdeimplbasic.h>

#define _SERDE_SPEC(TYPE, SER, DE) struct _SerdeSpec<TYPE> { const SerdePair value{SER, DE}; };
#define SERDE_SPEC(NS, TYPE) template<> _SERDE_SPEC(NS::TYPE, serialize ## TYPE, deserialize ## TYPE)

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

template<typename T>
constexpr bool isTrivialSerde() {
    return !serdePair<T>().first;
}

SERDE_SPEC(std, string)

} // namespace serde
} // namespace util
