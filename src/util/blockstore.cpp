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
		return std::make_unique<BlockStore>(blockSize);
}

std::unordered_map<size_t, std::unique_ptr<IBlockStore>> BlockStoreManager::stores;

size_t FakeBlockGuard<0>::blockSize() noexcept {
	return 0;
}

void FakeBlockGuard<0>::load(void*) noexcept {
}

void FakeBlockGuard<0>::store(const void*) noexcept {
}

size_t FakeBlockGuard<1>::blockSize() noexcept {
	return 1;
}

void FakeBlockGuard<1>::load(void* v) noexcept {
	*static_cast<uint8_t*>(v) = key;
}

void FakeBlockGuard<1>::store(const void* v) noexcept {
	key = *static_cast<const uint8_t*>(v);
}

size_t FakeBlockGuard<2>::blockSize() noexcept {
	return 2;
}

void FakeBlockGuard<2>::load(void* v) noexcept {
	*static_cast<uint16_t*>(v) = key;
}

void FakeBlockGuard<2>::store(const void* v) noexcept {
	key = *static_cast<const uint16_t*>(v);
}

size_t FakeBlockGuard<4>::blockSize() noexcept {
	return 4;
}

void FakeBlockGuard<4>::load(void* v) noexcept {
	*static_cast<uint32_t*>(v) = key;
}

void FakeBlockGuard<4>::store(const void* v) noexcept {
	key = *static_cast<const uint32_t*>(v);
}

#ifdef _LP64
size_t FakeBlockGuard<8>::blockSize() noexcept {
	return 8;
}

void FakeBlockGuard<8>::load(void* v) noexcept {
	*static_cast<uint64_t*>(v) = key;
}

void FakeBlockGuard<8>::store(const void* v) noexcept {
	key = *static_cast<const uint64_t*>(v);
}
#endif

AtomicBlockStore::~AtomicBlockStore() {
	freeMemory();
}

size_t AtomicBlockStore::blockSize() noexcept {
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
	assert(_length <= _capacity);
	size_t freeTailBlockCount = _capacity - _length;
	memset(static_cast<uint8_t*>(mem) + _length * _blockSize, 0, freeTailBlockCount * _blockSize);
}

BlockStore::BlockStore(size_t blockSize) noexcept : _blockSize(blockSize) { }

BlockStore::~BlockStore() {
	freeMemory();
}

size_t BlockStore::blockSize() noexcept {
	return _blockSize;
}

size_t BlockStore::allocBlock() {
	if (_length == _capacity)
		setCapacity(_capacity ? _capacity * 2 : 1);

	return _length++;
}

void BlockStore::freeBlock(size_t) {
	throw std::logic_error("not implemented");
}

void BlockStore::load(size_t key, void* v) {
	if (key >= _length)
		throw std::out_of_range("Invalid key");

	memcpy(v, static_cast<uint8_t*>(mem) + _blockSize * key, _blockSize);
}

void BlockStore::store(size_t key, const void* v) {
	if (key >= _length)
		throw std::out_of_range("Invalid key");

	memcpy(static_cast<uint8_t*>(mem) + _blockSize * key, v, _blockSize);
}

size_t BlockStore::length() noexcept {
	return _length;
}

size_t BlockStore::capacity() noexcept {
	return _capacity;
}

void BlockStore::setCapacity(size_t n) {
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

void BlockStore::freeMemory() noexcept {
	free(mem);
	mem = nullptr;
}

void BlockStore::zeroOutFreeTailMemory() noexcept {
	assert(_length <= _capacity);
	size_t freeTailBlockCount = _capacity - _length;
	memset(static_cast<uint8_t*>(mem) + _length * _blockSize, 0, freeTailBlockCount * _blockSize);
}

} // namespace util
