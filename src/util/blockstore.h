#pragma once

#include <cstddef>

namespace util {

// TODO split into different files

class IBlockStore {
public:
	virtual ~IBlockStore() { }

	virtual size_t blockSize() = 0;
	virtual void* getBlock() = 0; // TODO rename
	virtual void freeBlock(void* key) = 0;
	virtual void load(void* key, void* v) = 0;
	virtual void store(void* key, const void* v) = 0;
};

class ICapacityControl {
public:
	virtual ~ICapacityControl() { }

	virtual size_t length() = 0;
	virtual size_t capacity() = 0;
	virtual void setCapacity(size_t n) = 0;
};

class AtomicBlockStore : public virtual IBlockStore, public virtual ICapacityControl {
public:
	AtomicBlockStore();
	~AtomicBlockStore();

	size_t blockSize() override;
	void* getBlock() override;
	void freeBlock(void* key) override;
	void load(void* key, void* v) override;
	void store(void* key, const void* v) override;

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
	void* key{};

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
