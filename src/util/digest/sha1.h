#pragma once

#include <cstdint>
#include <array>

namespace util {
namespace digest {


struct SHA1 final {
private:
	uint32_t state[5];
	uint32_t count[2];
	uint8_t buffer[64];

	void transform(const uint8_t* data);

public:
	static constexpr uint32_t hashLength = 20;
	using hash_t = std::array<uint8_t, hashLength>;

	SHA1();
	void start();
	void put(uint8_t b);
	void put(const void* d, uint32_t length);
	hash_t finish();

	// convinience function
	hash_t digest(const void* d, uint32_t length);
};

} // namespace digest
} // namespace util
