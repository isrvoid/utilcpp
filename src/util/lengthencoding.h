#pragma once

#include <cstdint>

// this implementation currently only works on little-endian machines and is not thread safe

namespace util {

class LengthEncoding {
	LengthEncoding() = delete;

public:
	static constexpr unsigned int byteCountMax = 8;
	static constexpr unsigned int byteLengthMax = (1 << 6) - 1;
	static constexpr unsigned int shortLengthMax = (1 << 14) - 1;
	static constexpr uint64_t lengthMax = (static_cast<uint64_t>(1) << 62) - 1;

	static const uint8_t* read(const uint8_t* data, uint64_t* lengthOut) noexcept;
	static uint8_t* write(uint64_t length, uint8_t* data) noexcept;
};

} // namespace util
