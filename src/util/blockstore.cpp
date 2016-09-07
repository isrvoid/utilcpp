#include "blockstore.h"

#include <cstdint>

namespace util {

void FakeBlockGuard<1>::load(void* v) {
	*static_cast<uint8_t*>(v) = reinterpret_cast<size_t>(key);
}

void FakeBlockGuard<1>::store(const void* v) {
	reinterpret_cast<size_t&>(key) = *static_cast<const uint8_t*>(v);
}

size_t FakeBlockGuard<1>::blockSize() {
	return 1;
}

void FakeBlockGuard<2>::load(void* v) {
	*static_cast<uint16_t*>(v) = reinterpret_cast<size_t>(key);
}

void FakeBlockGuard<2>::store(const void* v) {
	reinterpret_cast<size_t&>(key) = *static_cast<const uint16_t*>(v);
}

size_t FakeBlockGuard<2>::blockSize() {
	return 2;
}

void FakeBlockGuard<4>::load(void* v) {
	*static_cast<uint32_t*>(v) = reinterpret_cast<size_t>(key);
}

void FakeBlockGuard<4>::store(const void* v) {
	reinterpret_cast<size_t&>(key) = *static_cast<const uint32_t*>(v);
}

size_t FakeBlockGuard<4>::blockSize() {
	return 4;
}

//BlockStore::BlockStore(size_t blockSize


} // namespace util
