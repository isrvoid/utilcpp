#include "types.h"

#include <cassert>

namespace nadam {

MessageInfo::MessageInfo(std::string name, uint32_t size, bool isVariableSize)
	: name{name}, size{size}, isVariableSize{isVariableSize},
	hash{constructHash(name, size, isVariableSize)} {
	assert(name.length());
}

util::sha1::hash MessageInfo::constructHash(const std::string& name, uint32_t size, bool isVariableSize) {
	util::sha1::Digest sha;
	// FIXME
	return sha.digest(nullptr, 0);
}

} // namespace nadam
