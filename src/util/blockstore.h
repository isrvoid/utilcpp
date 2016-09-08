#pragma once

#include <cstddef>

namespace util {

// TODO split into different files

class IBlockStore {
public:
	virtual ~IBlockStore() { }

	virtual size_t blockSize() = 0;
	virtual size_t allocBlock() = 0;
	virtual void freeBlock(size_t key) = 0;
	virtual void load(size_t key, void* v) = 0;
	virtual void store(size_t key, const void* v) = 0;
};

class ICapacityControl {
public:
	virtual ~ICapacityControl() { }

	virtual size_t length() = 0;
	virtual size_t capacity() = 0;
	virtual void setCapacity(size_t n) = 0;
};

class AtomicBlockStore : public virtual IBlockStore, public virtual ICapacityControl {
	void* mem{};
	size_t _length{};
	size_t _capacity{};
	static constexpr size_t _blockSize = 8;
	void freeMemory();
	void zeroOutFreeTailMemory();

public:
	~AtomicBlockStore();

	size_t blockSize() override;
	size_t allocBlock() override;
	void freeBlock(size_t key) override;
	void load(size_t key, void* v) override;
	void store(size_t key, const void* v) override;

	size_t length() override;
	size_t capacity() override;
	void setCapacity(size_t n);
};

// minimal block alignment is 4
// FIXME
class BlockStore : public virtual IBlockStore, public virtual ICapacityControl {
public:
	BlockStore(size_t blockSize);
	~BlockStore();
};

// abstract base class for BlockStore wrapper that manages a single block
class BlockGuard {
protected:
	size_t key{};

public:
	virtual ~BlockGuard() { }

	virtual size_t blockSize() = 0;
	virtual void load(void* v) = 0;
	virtual void store(const void* v) = 0;
};

template<size_t block_size>
class FakeBlockGuard : public BlockGuard {
};

template<>
class FakeBlockGuard<1> : public BlockGuard {
public:
	void load(void* v) override;
	void store(const void* v) override;
	size_t blockSize() override;
};

template<>
class FakeBlockGuard<2> : public BlockGuard {
public:
	void load(void* v) override;
	void store(const void* v) override;
	size_t blockSize() override;
};

template<>
class FakeBlockGuard<4> : public BlockGuard {
public:
	void load(void* v) override;
	void store(const void* v) override;
	size_t blockSize() override;
};

class BlockGuardImpl : public BlockGuard {
private:
	IBlockStore* store{};

public:
	BlockGuardImpl(IBlockStore* store);
	~BlockGuardImpl();
};

} // namespace util
