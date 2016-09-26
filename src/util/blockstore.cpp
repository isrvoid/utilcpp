#include "blockstore.h"

#include <stdexcept>
#include <cstdint>
#include <cstring>
#include <cassert>

namespace util {

IBlockStore& BlockStoreManager::instance(size_t blockSize) noexcept {
	assert(blockSize > sizeof(void*) && blockSize % 4 == 0);
	auto& p = stores[blockSize];
	if (p.get() == nullptr)
		p = createInstance(blockSize);
	return *p;
}

std::unique_ptr<IBlockStore> BlockStoreManager::createInstance(size_t blockSize) noexcept {
	if (blockSize == sizeof(MaxAtomic))
		return std::make_unique<AtomicBlockStore>();
	else
		return std::make_unique<AtomicBlockStore>(); // FIXME
}

std::unordered_map<size_t, std::unique_ptr<IBlockStore>> BlockStoreManager::stores;

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

#ifdef _LP64
size_t FakeBlockGuard<8>::blockSize() {
	return 8;
}

void FakeBlockGuard<8>::load(void* v) {
	*static_cast<uint64_t*>(v) = key;
}

void FakeBlockGuard<8>::store(const void* v) {
	key = *static_cast<const uint64_t*>(v);
}
#endif

AtomicBlockStore::~AtomicBlockStore() {
	freeMemory();
}

size_t AtomicBlockStore::blockSize() noexcept {
	return _blockSize;
}

size_t AtomicBlockStore::allocBlock() noexcept {
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

	*static_cast<MaxAtomic*>(v) = static_cast<MaxAtomic*>(mem)[key];
}

void AtomicBlockStore::store(size_t key, const void* v) {
	if (key >= _length)
		throw std::out_of_range("Invalid key");

	static_cast<MaxAtomic*>(mem)[key] = *static_cast<const MaxAtomic*>(v);
}

size_t AtomicBlockStore::length() noexcept {
	return _length;
}

size_t AtomicBlockStore::capacity() noexcept {
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

void AtomicBlockStore::freeMemory() noexcept {
	free(mem);
	mem = nullptr;
}

void AtomicBlockStore::zeroOutFreeTailMemory() noexcept {
	size_t indexAfterLastBlock = _length;
	assert(indexAfterLastBlock <= _capacity);
	size_t freeTailBlockCount = _capacity - indexAfterLastBlock;
	memset(static_cast<uint8_t*>(mem) + indexAfterLastBlock * _blockSize, 0, freeTailBlockCount * _blockSize);
}

} // namespace util
