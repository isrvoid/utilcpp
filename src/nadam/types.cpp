#include "types.h"

#include <cassert>

namespace nadam {

MessageInfo::MessageInfo(std::string name, uint32_t size, bool isVariableSize)
	: name(name), size(size), isVariableSize(isVariableSize),
	hash(constructHash(name, size, isVariableSize)) {
	assert(name.length());
}

MessageInfo::hash_t MessageInfo::constructHash(const std::string& name, uint32_t size, bool isVariableSize) {
	util::digest::SHA1 sha;
	sha.put(name.data(), static_cast<uint32_t>(name.size()));
	sha.put(isVariableSize);
	for (size_t i = 0; i < sizeof(size); ++i) {
		size_t byteNumber = sizeof(size) - 1 - i;
		sha.put(size >> 8 * byteNumber & 0xff);
	}
	return sha.finish();
}

} // namespace nadam
