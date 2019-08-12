/*
Copyright (c) 2016 Johannes Teichrieb
License: opensource.org/licenses/MIT
*/
#pragma once

#include <cstdint>
#include <string>

#include <util/digest/sha1.h>

namespace nadam {

struct MessageInfo {
	const std::string name;
	union {
		const uint32_t size;
		const uint32_t maxSize;
	};
	const bool isVariableSize;
	using hash_t = util::digest::SHA1::hash_t;
	const hash_t hash;

	MessageInfo(std::string name, uint32_t size, bool isVariableSize = false);

private:
	hash_t constructHash(const std::string& name, uint32_t size, bool isVariableSize);
};

} // namespace nadam
