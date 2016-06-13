#pragma once

#include <cstdint>
#include <array>

namespace util {
namespace sha1 {

constexpr int hashLength = 20;
using hash = std::array<uint8_t, hashLength>;

struct Digest final {
private:
	uint32_t state[5];
	uint32_t count[2];
	uint8_t buffer[64];

	void transform(const uint8_t* data);

public:
	Digest();
	void start();
	void put(uint8_t b);
	void put(const void* d, uint32_t length);
	hash finish();

	// convinience function
	hash digest(const void* d, uint32_t length);
};

} // namespace sha1
} // namespace util
