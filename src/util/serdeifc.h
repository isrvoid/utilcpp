#pragma once

namespace util {
namespace serde {

// The interface is tuned for reuse and avoids boilerplate length calculations.
// Looking at implementation examples helps understand the decisions behind it.

// Serialization functions should not throw.

// Upon success, serialize functions shall return end of serialized data.
// Otherwise, nullptr shall be returned, indicating insufficinet destination capacity.
using serializeFun = void* (*)(const void* obj, void* dest, const void* destEnd);

// Upon success, deserialize function shall return end of consumed src. Otherwise, nullptr shall be returned.
using deserializeFun = const void* (*)(const void* src, const void* srcEnd, void* sharedPtrOut);

} // namespace serde
} // namespace util
