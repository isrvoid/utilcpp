#include "blockstore.h"

#include <stdexcept>
#include <cstdint>
#include <cstring>
#include <cassert>
#include <thread>

namespace util {

IBlockStore& BlockStoreManager::instance(size_t blockSize) noexcept {
	assert(blockSize > sizeof(void*) && blockSize % 4 == 0);
	auto& p = stores[blockSize];
	if (p.get() == nullptr)
		p = createInstance(blockSize);
	return *p;
}

void BlockStoreManager::deleteAllInstances() noexcept {
	stores.clear();
}

std::unique_ptr<IBlockStore> BlockStoreManager::createInstance(size_t blockSize) noexcept {
	if (blockSize == sizeof(MaxAtomic))
		return std::make_unique<AtomicBlockStore>();
	else
		return std::make_unique<BlockStore>(blockSize);
}

std::unordered_map<size_t, std::unique_ptr<IBlockStore>> BlockStoreManager::stores;

BlockStoreManager::iterator::iterator(const decltype(it)& it) noexcept : it(it) { }

bool BlockStoreManager::iterator::operator!=(const iterator& other) const noexcept {
	return it != other.it;
}

BlockStoreManager::iterator& BlockStoreManager::iterator::operator++() noexcept {
	++it; return *this;
}

BlockStoreManager::iterator::reference BlockStoreManager::iterator::operator*() const noexcept {
	return *it->second;
}

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

PlainBlockGuard::PlainBlockGuard(IBlockStore& store) : _store(store) {
	key = _store.allocBlock();
}

PlainBlockGuard::~PlainBlockGuard() {
	// _store.freeBlock(key); // TODO uncomment after it's implemented
}

void PlainBlockGuard::load(void* v) noexcept {
	_store.load(key, v);
}

void PlainBlockGuard::store(const void* v) noexcept {
	_store.store(key, v);
}

size_t PlainBlockGuard::blockSize() noexcept {
	return _store.blockSize();
}

LockingBlockGuard::LockingBlockGuard(IBlockStore& store) : _store(store) {
	key = _store.allocBlock();
}

LockingBlockGuard::~LockingBlockGuard() {
	// _store.freeBlock(key); // TODO uncomment after it's implemented
}

void LockingBlockGuard::load(void* v) noexcept {
	LockGuard lock(_lock);
	_store.load(key, v);
}

void LockingBlockGuard::store(const void* v) noexcept {
	LockGuard lock(_lock);
	_store.store(key, v);
}

size_t LockingBlockGuard::blockSize() noexcept {
	return _store.blockSize();
}

LockingBlockGuard::LockGuard::LockGuard(std::atomic<bool>& lock) : lock(lock) {
	bool expected;
	while (expected = false, lock.load(std::memory_order_acquire) || !std::atomic_compare_exchange_weak_explicit(&lock, &expected, true, std::memory_order_release, std::memory_order_relaxed)) {
		std::this_thread::yield();
	}
}

LockingBlockGuard::LockGuard::~LockGuard() {
	lock.store(false, std::memory_order_release);
}

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
