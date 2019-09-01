#pragma once

#include <cstddef>

namespace util {

// The interface is tuned for reuse and avoids boilerplate length calculations.
// Looking at implementation examples helps understand the decisions behind it.

// Serialization functions should not throw.

// Upon success, serialize functions shall return end of serialized data.
// Otherwise, nullptr shall be returned, indicating insufficinet destination capacity.
using serializeFunc = void* (*)(void* dest, const void* destEnd, const void* obj);

// Upon success, deserialize function shall return end of consumed src. Otherwise, nullptr shall be returned.
using deserializeFunc = const void* (*)(void* sharedDest, const void* src, const void* srcEnd);

} // namespace util
