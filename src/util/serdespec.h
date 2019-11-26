#pragma once

// serdespecbasic.h is not included to allow extensions
#include <util/serdeimpl.h>

#undef SERDE_SPEC
#define SERDE_SPEC(NS, TYPE) template<typename T> _SERDE_SPEC(NS::TYPE, serialize ## TYPE, deserialize ## TYPE)

namespace util {
namespace serde {

SERDE_SPEC(std, vector<T>)

} // namespace serde
} // namespace util
