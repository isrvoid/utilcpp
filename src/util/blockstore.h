#pragma once

#include <cstddef>

namespace util {

class IBlockStore {
public:
	virtual ~IBlockStore() { }

	virtual void* getBlock() = 0;
	virtual void freeBlock(void* key) = 0;
	virtual void load(void* key, void* v) = 0;
	virtual void store(void* key, const void* v) = 0;
	virtual size_t blockSize() = 0;
};

class ICapacityControl {
public:
	virtual ~ICapacityControl() { }

	virtual size_t capacity() = 0;
	virtual size_t length() = 0;
	virtual void setCapacity(size_t n);
};

// minimal block alignment is 4
class BlockStore : public virtual IBlockStore, public virtual ICapacityControl {
	BlockStore(size_t blockSize);
	~BlockStore();
};

// abstract base class for BlockStore wrapper that manages a single block
class BlockGuard {
protected:
	void* key{};

public:
	virtual ~BlockGuard() { }

	virtual void load(void* v) = 0;
	virtual void store(const void* v) = 0;
	virtual size_t blockSize() = 0;
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

} // namespace util
