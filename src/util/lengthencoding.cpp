#include "lengthencoding.h"

#include <cassert>

namespace util {

const uint8_t* LengthEncoding::read(const uint8_t* data, uint64_t* lengthOut) noexcept {
	const unsigned int byteCount = 1 << (*data >> 6);
	uint64_t length = 0;
	uint8_t* p = reinterpret_cast<uint8_t*>(&length);

	switch (byteCount) {
		case 8: *p++ = data[7];
				*p++ = data[6];
				*p++ = data[5];
				*p++ = data[4];
		case 4: *p++ = data[3]; // this case is never encoded, but can be decoded
				*p++ = data[2];
		case 2: *p++ = data[1];
		case 1: *p = data[0] & 0x3f;
	}
	*lengthOut = length;
	return data + byteCount;
}

uint8_t* LengthEncoding::write(uint64_t length, uint8_t* data) noexcept {
	assert(length <= lengthMax);
	const unsigned int byteCountMask = (length > shortLengthMax) << 7 | (length > byteLengthMax) << 6;
	const unsigned int byteCount = 1 << (byteCountMask >> 6);
	uint8_t* p = reinterpret_cast<uint8_t*>(&length);
	switch (byteCount) {
		case 8: data[7] = *p++;
				data[6] = *p++;
				data[5] = *p++;
				data[4] = *p++;
				data[3] = *p++;
				data[2] = *p++;
		case 2: data[1] = *p++;
		case 1: data[0] = *p | byteCountMask;
	}

	return data + byteCount;
}

uint8_t* LengthEncoding::writeReverse(uint64_t length, uint8_t* data) noexcept {
	assert(length <= lengthMax);
	const unsigned int byteCountMask = (length > shortLengthMax) << 7 | (length > byteLengthMax) << 6;
	const unsigned int byteCount = 1 << (byteCountMask >> 6);
	uint8_t* p = reinterpret_cast<uint8_t*>(&length);
	switch (byteCount) {
		case 8: *--data = *p++;
				*--data = *p++;
				*--data = *p++;
				*--data = *p++;
				*--data = *p++;
				*--data = *p++;
		case 2: *--data = *p++;
		case 1: *--data = *p | byteCountMask;
	}
	return data;
}

} // namespace util
