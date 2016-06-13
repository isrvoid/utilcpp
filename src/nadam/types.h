/*
Copyright (c) 2016 Johannes Teichrieb
License: opensource.org/licenses/MIT
*/
#pragma once

#include <cstdint>
#include <string>

#include <util/sha1/digest.h>

namespace nadam {

struct MessageInfo {
	const std::string name;
	union {
		const uint32_t size;
		const uint32_t maxSize;
	};
	const bool isVariableSize;
	const util::sha1::hash hash;

	MessageInfo(std::string name, uint32_t size, bool isVariableSize = false);

	private:
	util::sha1::hash constructHash(const std::string& name, uint32_t size, bool isVariableSize);
};

} // namespace nadam
