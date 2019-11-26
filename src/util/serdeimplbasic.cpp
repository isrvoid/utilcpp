#include "serdeimplbasic.h"

#include <cstring>
#include <memory>
#include <string>

using namespace std;

namespace util {
namespace serde {

void* serializestring(const void* _p, void* dest, const void* destEnd) noexcept {
	auto p = static_cast<const string*>(_p);
    size_t length = p->length();
	if (static_cast<uint8_t*>(dest) + 4 + length > destEnd)
		return nullptr;

    writeUint(length, dest);
	memcpy(dest, p->data(), length);
	return static_cast<uint8_t*>(dest) + length;
}

const void* deserializestring(const void* src, const void* srcEnd, void* ptrOut) noexcept {
	if (static_cast<const uint8_t*>(src) + 4 > srcEnd)
		return nullptr;

	uint32_t length;
    readUint(src, length);
	if (static_cast<const uint8_t*>(src) + length > srcEnd)
		return nullptr;

    auto p = make_shared<string>(static_cast<const char*>(src), length);
	*static_cast<decltype(p)*>(ptrOut) = p;
	return static_cast<const uint8_t*>(src) + length;
}

} // namespace serde
} // namespace util
