#include "blockstore.h"

#include <stdexcept>
#include <cstdint>
#include <cstring>
#include <cassert>

namespace util {

size_t FakeBlockGuard<0>::blockSize() {
	return 0;
}

void FakeBlockGuard<0>::load(void*) {
}

void FakeBlockGuard<0>::store(const void*) {
}

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

void AtomicBlockStore::freeMemory() {
	free(mem);
	mem = nullptr;
}

void AtomicBlockStore::zeroOutFreeTailMemory() {
	size_t indexAfterLastBlock = _length;
	assert(indexAfterLastBlock <= _capacity);
	size_t freeTailBlockCount = _capacity - indexAfterLastBlock;
	memset(static_cast<uint8_t*>(mem) + indexAfterLastBlock * _blockSize, 0, freeTailBlockCount * _blockSize);
}

AtomicBlockStore::~AtomicBlockStore() {
	freeMemory();
}

size_t AtomicBlockStore::blockSize() {
	return _blockSize;
}

size_t AtomicBlockStore::allocBlock() {
	if (_length == _capacity)
		setCapacity(_capacity ? _capacity * 2 : 1);

	return _length++;
}

void AtomicBlockStore::freeBlock(size_t) {
	throw std::logic_error("not implemented");
}

void AtomicBlockStore::load(size_t key, void* v) {
	if (key >= _length)
		throw std::out_of_range("Invalid key");

	*static_cast<uint64_t*>(v) = static_cast<uint64_t*>(mem)[key];
}

void AtomicBlockStore::store(size_t key, const void* v) {
	if (key >= _length)
		throw std::out_of_range("Invalid key");

	static_cast<uint64_t*>(mem)[key] = *static_cast<const uint64_t*>(v);
}

size_t AtomicBlockStore::length() {
	return _length;
}

size_t AtomicBlockStore::capacity() {
	return _capacity;
}

void AtomicBlockStore::setCapacity(size_t n) {
	if (n < _length)
		throw std::length_error("Capacity must be greater than or equal to length");

	if (n == _capacity)
		return;

	if (n == 0) {
		freeMemory();
		return;
	}

	auto newMem = realloc(mem, _blockSize * n);
	if (!newMem)
		throw new std::runtime_error("realloc() failed");

	mem = newMem;
	_capacity = n;

	zeroOutFreeTailMemory();
}

} // namespace util
