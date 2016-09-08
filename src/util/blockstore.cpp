#include "blockstore.h"

#include <cstdint>

namespace util {

size_t FakeBlockGuard<1>::blockSize() {
	return 1;
}

void FakeBlockGuard<1>::load(void* v) {
	*static_cast<uint8_t*>(v) = key;
}

void FakeBlockGuard<1>::store(const void* v) {
	key = *static_cast<const uint8_t*>(v);
}

size_t FakeBlockGuard<2>::blockSize() {
	return 2;
}

void FakeBlockGuard<2>::load(void* v) {
	*static_cast<uint16_t*>(v) = key;
}

void FakeBlockGuard<2>::store(const void* v) {
	key = *static_cast<const uint16_t*>(v);
}

size_t FakeBlockGuard<4>::blockSize() {
	return 4;
}

void FakeBlockGuard<4>::load(void* v) {
	*static_cast<uint32_t*>(v) = key;
}

void FakeBlockGuard<4>::store(const void* v) {
	key = *static_cast<const uint32_t*>(v);
}

// FIXME
AtomicBlockStore::AtomicBlockStore() {
}

AtomicBlockStore::~AtomicBlockStore() {
}

size_t AtomicBlockStore::blockSize() {
	return 8;
}

size_t AtomicBlockStore::allocBlock() {
	return 0;
}

void AtomicBlockStore::freeBlock(size_t) {
}

void AtomicBlockStore::load(size_t, void*) {
}

void AtomicBlockStore::store(size_t, const void*) {
}

size_t AtomicBlockStore::length() {
	return 0;
}

size_t AtomicBlockStore::capacity() {
	return 0;
}

void AtomicBlockStore::setCapacity(size_t) {
}

} // namespace util
