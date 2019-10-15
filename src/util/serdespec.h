#pragma once

#include <cstdint>
#include <utility>
#include <string>
#include <vector>

#include <util/typeinfo.h>
#include <util/serde.h>
#include <util/serdeimpl.h>

#define _SERDE_SPEC(TYPE, SER, DE)  template<> struct Spec<TypeHash<TYPE>::hash> { \
    static constexpr std::pair<serializeFun, deserializeFun> value{SER, DE}; }

#define SERDE_SPEC(NS, TYPE)  _SERDE_SPEC(NS::TYPE, serialize ## TYPE, deserialize ## TYPE)

namespace util {
namespace serde {

template<uint32_t>
struct Spec {
    static constexpr void* value{nullptr};
};

SERDE_SPEC(std, vector<uint8_t>);

} // namespace serde
} // namespace util
