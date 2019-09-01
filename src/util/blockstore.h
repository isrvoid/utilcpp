#pragma once

#include <cstddef>
#include <memory>
#include <unordered_map>
#include <atomic>
#include <iterator>

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

class BlockStoreManager {
    static std::unordered_map<size_t, std::unique_ptr<IBlockStore>> stores;

    static std::unique_ptr<IBlockStore> createInstance(size_t blockSize) noexcept;

public:
    static IBlockStore& instance(size_t blockSize) noexcept;

    static void deleteAllInstances() noexcept; // for unittest

    class iterator : public std::iterator<std::forward_iterator_tag, IBlockStore> {
        decltype(stores.begin()) it;

    public:
        explicit iterator(const decltype(it)& it) noexcept;
        bool operator!=(const iterator& other) const noexcept;
        iterator& operator++() noexcept;
        reference operator*() const noexcept;
    };

    static iterator begin() noexcept;
    static iterator end() noexcept;
};

// this only works for machines that can set values of size 2*sizeof(void*) atomically (e.g. armv7a, x86 MMX)
template<size_t pointer_size>
struct MaxAtomicForPointerSize {
    uint64_t _dummy[2 * sizeof(void*) / 8];
};

using MaxAtomic = MaxAtomicForPointerSize<sizeof(void*)>;

class AtomicBlockStore : public virtual IBlockStore, public virtual ICapacityControl {
    size_t _length{};
    size_t _capacity{};
    static constexpr size_t _blockSize = sizeof(MaxAtomic);

    void freeMemory() noexcept;
    void zeroOutFreeTailMemory() noexcept;

    AtomicBlockStore(const AtomicBlockStore&) = delete;

public:
    void* mem{};
    AtomicBlockStore() = default;
    ~AtomicBlockStore();
    AtomicBlockStore(AtomicBlockStore&&) = default;

    size_t blockSize() noexcept override;
    size_t allocBlock() override;
    void freeBlock(size_t key) override;
    void load(size_t key, void* v) noexcept override;
    void store(size_t key, const void* v) noexcept override;

    size_t length() noexcept override;
    size_t capacity() noexcept override;
    void setCapacity(size_t n) override;
};

// minimal block alignment is 4
class BlockStore : public virtual IBlockStore, public virtual ICapacityControl {
    void* mem{};
    size_t _length{};
    size_t _capacity{};
    const size_t _blockSize;

    void freeMemory() noexcept;
    void zeroOutFreeTailMemory() noexcept;

    BlockStore(const BlockStore&) = delete;

public:
    BlockStore(size_t blockSize) noexcept;
    ~BlockStore();
    BlockStore(BlockStore&&) = default;

    size_t blockSize() noexcept override;
    size_t allocBlock() override;
    void freeBlock(size_t key) override;
    void load(size_t key, void* v) noexcept override;
    void store(size_t key, const void* v) noexcept override;

    size_t length() noexcept override;
    size_t capacity() noexcept override;
    void setCapacity(size_t n) override;
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
class FakeBlockGuard<0> : public BlockGuard {
public:
    void load(void* v) noexcept override;
    void store(const void* v) noexcept override;
    size_t blockSize() noexcept override;
};

template<>
class FakeBlockGuard<1> : public BlockGuard {
public:
    void load(void* v) noexcept override;
    void store(const void* v) noexcept override;
    size_t blockSize() noexcept override;
};

template<>
class FakeBlockGuard<2> : public BlockGuard {
public:
    void load(void* v) noexcept override;
    void store(const void* v) noexcept override;
    size_t blockSize() noexcept override;
};

template<>
class FakeBlockGuard<4> : public BlockGuard {
public:
    void load(void* v) noexcept override;
    void store(const void* v) noexcept override;
    size_t blockSize() noexcept override;

};

#ifdef _LP64
template<>
class FakeBlockGuard<8> : public BlockGuard {
public:
    void load(void* v) noexcept override;
    void store(const void* v) noexcept override;
    size_t blockSize() noexcept override;
};
#endif

class AtomicBlockGuard : public BlockGuard {
private:
    AtomicBlockStore& _store;

public:
    AtomicBlockGuard(AtomicBlockStore& store);
    ~AtomicBlockGuard();

    void load(void* v) noexcept override;
    void store(const void* v) noexcept override;
    size_t blockSize() noexcept override;
};

class LockingBlockGuard : public BlockGuard {
private:
    IBlockStore& _store;
    std::atomic<uint8_t> _lock{}; // mutex is to big to be used here

    struct LockGuardLoad {
        std::atomic<uint8_t>& lock;

        LockGuardLoad(std::atomic<uint8_t>& lock) noexcept;
        ~LockGuardLoad();
    };

    struct LockGuardStore {
        std::atomic<uint8_t>& lock;

        LockGuardStore(std::atomic<uint8_t>& lock) noexcept;
        ~LockGuardStore();
    };

public:
    LockingBlockGuard(IBlockStore& store);
    ~LockingBlockGuard();

    void load(void* v) noexcept override;
    void store(const void* v) noexcept override;
    size_t blockSize() noexcept override;
};

// for the destructor to work SharedPtrBlockGuard should have exclusive ownership
// over its block within the store
class SharedPtrBlockGuard : public BlockGuard {
private:
    AtomicBlockStore& _store;

public:
    SharedPtrBlockGuard(AtomicBlockStore& store);
    ~SharedPtrBlockGuard();

    // load should be used carefully
    // it increments use_count and can lead to memory never being freed
    void load(void* v) noexcept override;
    void store(const void* v) noexcept override;
    size_t blockSize() noexcept override;
};

} // namespace util
