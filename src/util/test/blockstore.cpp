#include <gtest/gtest.h>

#include <stdio.h>
#include <memory>
#include <atomic>
#include <cstring>
#include <array>
#include <algorithm>

#include <util/blockstore.h>

using namespace std;

namespace util {

class FakeBlockGuardTest : public ::testing::Test {
protected:
	unique_ptr<BlockGuard> block0 = make_unique<FakeBlockGuard<0>>();
	unique_ptr<BlockGuard> block1 = make_unique<FakeBlockGuard<1>>();
	unique_ptr<BlockGuard> block2 = make_unique<FakeBlockGuard<2>>();
	unique_ptr<BlockGuard> block4 = make_unique<FakeBlockGuard<4>>();
#ifdef _LP64
	unique_ptr<BlockGuard> block8 = make_unique<FakeBlockGuard<8>>();
#endif
};

TEST_F(FakeBlockGuardTest, BlockSize0) {
	ASSERT_EQ(0, block0->blockSize());
}

TEST_F(FakeBlockGuardTest, BlockSize1) {
	ASSERT_EQ(1, block1->blockSize());
}

TEST_F(FakeBlockGuardTest, BlockSize2) {
	ASSERT_EQ(2, block2->blockSize());
}

TEST_F(FakeBlockGuardTest, BlockSize4) {
	ASSERT_EQ(4, block4->blockSize());
}

TEST_F(FakeBlockGuardTest, Load0IsIgnored) {
	double val = 42.4242;
	block0->load(&val);
	ASSERT_EQ(42.4242, val);
}

TEST_F(FakeBlockGuardTest, InitValue1) {
	uint8_t val;
	block1->load(&val);
	ASSERT_EQ(0, val);
}

TEST_F(FakeBlockGuardTest, InitValue2) {
	uint16_t val;
	block2->load(&val);
	ASSERT_EQ(0, val);
}

TEST_F(FakeBlockGuardTest, InitValue4) {
	uint32_t val;
	block4->load(&val);
	ASSERT_EQ(0, val);
}

TEST_F(FakeBlockGuardTest, Store0IsIgnored) {
	const uint8_t store = 0xbd;
	double load = 1.23456789;
	block0->store(&store);
	block0->load(&load);
	ASSERT_EQ(1.23456789, load);
}

TEST_F(FakeBlockGuardTest, Store1) {
	const uint8_t store = 0xbd;
	uint8_t load;
	block1->store(&store);
	block1->load(&load);
	ASSERT_EQ(store, load);
}

TEST_F(FakeBlockGuardTest, Store2) {
	const uint16_t store = 0xa55a;
	uint16_t load;
	block2->store(&store);
	block2->load(&load);
	ASSERT_EQ(store, load);
}

TEST_F(FakeBlockGuardTest, Store4) {
	const uint32_t store = 0x5aaaaa55;
	uint32_t load;
	block4->store(&store);
	block4->load(&load);
	ASSERT_EQ(store, load);
}

#ifdef _LP64
TEST_F(FakeBlockGuardTest, BlockSize8) {
	ASSERT_EQ(8, block8->blockSize());
}

TEST_F(FakeBlockGuardTest, InitValue8) {
	uint64_t val;
	block8->load(&val);
	ASSERT_EQ(0, val);
}

TEST_F(FakeBlockGuardTest, Store8) {
	const uint64_t store = 0x5aaaaaaaaaaaaa55;
	uint64_t load;
	block8->store(&store);
	block8->load(&load);
	ASSERT_EQ(store, load);
}
#endif

template<typename T, size_t Length>
bool equal(const T(&lhs)[Length], const T(&rhs)[Length]) {
	return !memcmp(lhs, rhs, sizeof(T) * Length);
}

class PlainBlockGuardTest : public ::testing::Test {
protected:
	static constexpr size_t blockSize = sizeof(MaxAtomic);
	AtomicBlockStore store;
	PlainBlockGuard _block{store};
	BlockGuard& block = _block;
};

TEST_F(PlainBlockGuardTest, BlockSize) {
	ASSERT_EQ(store.blockSize(), block.blockSize());
}

TEST_F(PlainBlockGuardTest, InstantiationGrabsAKey) {
	ASSERT_EQ(1, store.length());
	PlainBlockGuard anotherBlock{store};
	ASSERT_EQ(2, store.length());
}

TEST_F(PlainBlockGuardTest, Load) {
	uint8_t val[blockSize];
	memset(val, 0x95, blockSize);
	uint8_t init[blockSize]{};
	block.load(val);
	ASSERT_TRUE(equal(init, val));
}

TEST_F(PlainBlockGuardTest, Store) {
	uint8_t val[blockSize];
	memset(val, 0x95, blockSize);
	uint8_t verify[blockSize];
	block.store(val);
	block.load(verify);
	ASSERT_TRUE(equal(val, verify));
}

class LockingBlockGuardTest : public ::testing::Test {
protected:
	static constexpr size_t blockSize = 32;
	BlockStore store{blockSize};
	LockingBlockGuard _block{store};
	BlockGuard& block = _block;
};

TEST_F(LockingBlockGuardTest, BlockSize) {
	ASSERT_EQ(store.blockSize(), block.blockSize());
}

TEST_F(LockingBlockGuardTest, InstantiationGrabsAKey) {
	ASSERT_EQ(1, store.length());
	LockingBlockGuard anotherBlock{store};
	ASSERT_EQ(2, store.length());
}

TEST_F(LockingBlockGuardTest, Load) {
	uint8_t val[blockSize];
	memset(val, 0x95, blockSize);
	uint8_t init[blockSize]{};
	block.load(val);
	ASSERT_TRUE(equal(init, val));
}

TEST_F(LockingBlockGuardTest, Store) {
	uint8_t val[blockSize];
	memset(val, 0x95, blockSize);
	uint8_t verify[blockSize];
	block.store(val);
	block.load(verify);
	ASSERT_TRUE(equal(val, verify));
}

bool operator==(const MaxAtomic& lhs, const MaxAtomic& rhs) {
	return !memcmp(&lhs, &rhs, sizeof(lhs));
}

TEST(MaxAtomicTest, AssignmentAffectsAllBytes) {
	// more of a sanity check
	MaxAtomic expected, v;
	memset(&expected, 0x5a, sizeof(expected));
	v = expected;
	ASSERT_EQ(expected, v);
}

// platform dependent -- relevant for lock free operations on shared_ptr for example
TEST(MaxAtomicTest, IsTwiceThePointerSize) {
	ASSERT_EQ(2 * sizeof(void*), sizeof(MaxAtomic));
}

TEST(MaxAtomicTest, IsLockFree) {
	atomic<MaxAtomic> v;
	ASSERT_TRUE(atomic_is_lock_free(&v));
}

class AtomicBlockStoreTest : public ::testing::Test {
protected:
	AtomicBlockStore store;
};

TEST_F(AtomicBlockStoreTest, BlockSize) {
	store.blockSize();
}

TEST_F(AtomicBlockStoreTest, BlockSizeIsMaxAtomic) {
	ASSERT_EQ(sizeof(MaxAtomic), store.blockSize());
}

TEST_F(AtomicBlockStoreTest, Length) {
	ASSERT_EQ(0, store.length());
}

TEST_F(AtomicBlockStoreTest, AllocBlock) {
	store.allocBlock();
}

TEST_F(AtomicBlockStoreTest, AllocationIncreasesLength) {
	store.allocBlock();
	ASSERT_EQ(1, store.length());
	store.allocBlock();
	ASSERT_EQ(2, store.length());
}

class AtomicBlockStoreTest2 : public AtomicBlockStoreTest {
protected:
	size_t key1, key2;

	AtomicBlockStoreTest2() {
		key1 = store.allocBlock();
		key2 = store.allocBlock();
	}
};

TEST_F(AtomicBlockStoreTest2, InitValue) {
	const MaxAtomic init{};
	MaxAtomic val;

	store.load(key1, &val);
	ASSERT_EQ(init, val);
	store.load(key2, &val);
	ASSERT_EQ(init, val);
}

TEST_F(AtomicBlockStoreTest2, Store) {
	MaxAtomic test1, test2;
	memset(&test1, 0xa5, sizeof(MaxAtomic));
	memset(&test2, 0x96, sizeof(MaxAtomic));
	MaxAtomic val;

	store.store(key1, &test1);
	store.store(key2, &test2);

	store.load(key1, &val);
	ASSERT_EQ(test1, val);

	store.load(key2, &val);
	ASSERT_EQ(test2, val);
}

// freeing would require bookkeeping of allocated blocks -- initial implementation can get by without it
// TODO delete this test and comment in following tests after implementing freeBlock()
TEST_F(AtomicBlockStoreTest, FreeBlockThrows) {
	auto key = store.allocBlock();
	ASSERT_THROW(store.freeBlock(key), logic_error);
}

/*
TEST_F(AtomicBlockStoreTest, FreeBlock) {
	auto key = store.allocBlock();
	store.freeBlock(key);
}

TEST_F(AtomicBlockStoreTest, FreeingDecreasesLength) {
	auto key1 = store.allocBlock();
	auto key2 = store.allocBlock();
	store.freeBlock(key1);
	ASSERT_EQ(1, store.length());
	store.freeBlock(key2);
	ASSERT_EQ(0, store.length());
}
*/

TEST_F(AtomicBlockStoreTest, Capacity) {
	store.capacity();
}

TEST_F(AtomicBlockStoreTest, AllocIncreasesCapacityIfNoSpaceIsAvailable) {
	auto initialCapacity = store.capacity();
	while (store.length() < store.capacity())
		store.allocBlock();

	store.allocBlock();
	ASSERT_LT(initialCapacity, store.capacity());
}

TEST_F(AtomicBlockStoreTest, SetCapacity) {
	auto newCapacity = store.capacity() + 3;
	store.setCapacity(newCapacity);
	ASSERT_LE(newCapacity, store.capacity());
}

TEST_F(AtomicBlockStoreTest, SettingCapacityBelowLengthThrows) {
	store.allocBlock();
	ASSERT_THROW(store.setCapacity(0), length_error);
}

class BlockStoreTest : public ::testing::Test {
protected:
	static constexpr size_t blockSize = 40;
	BlockStore store = BlockStore(blockSize);
};

TEST_F(BlockStoreTest, BlockSize) {
	auto size = blockSize;
	ASSERT_EQ(size, store.blockSize());
}

TEST_F(BlockStoreTest, Length) {
	ASSERT_EQ(0, store.length());
}

TEST_F(BlockStoreTest, AllocBlock) {
	store.allocBlock();
}

TEST_F(BlockStoreTest, AllocationIncreasesLength) {
	store.allocBlock();
	ASSERT_EQ(1, store.length());
	store.allocBlock();
	ASSERT_EQ(2, store.length());
}

class BlockStoreTest2 : public BlockStoreTest {
protected:
	size_t key1, key2;

	BlockStoreTest2() {
		key1 = store.allocBlock();
		key2 = store.allocBlock();
	}
};

TEST_F(BlockStoreTest2, InitValue) {
	const uint32_t init[blockSize / 4]{};
	uint32_t val[blockSize / 4];

	store.load(key1, &val);
	ASSERT_TRUE(equal(init, val));
	store.load(key2, &val);
	ASSERT_TRUE(equal(init, val));
}

TEST_F(BlockStoreTest2, Store) {
	uint8_t test1[blockSize], test2[blockSize];
	memset(&test1, 0xa5, sizeof(test1));
	memset(&test2, 0x96, sizeof(test2));
	uint8_t val[blockSize];

	store.store(key1, &test1);
	store.store(key2, &test2);

	store.load(key1, &val);
	ASSERT_TRUE(equal(test1, val));

	store.load(key2, &val);
	ASSERT_TRUE(equal(test2, val));
}

// TODO same as for AtomicBlockStore above -- delete after implementing freeBlock()
TEST_F(BlockStoreTest, FreeBlockThrows) {
	auto key = store.allocBlock();
	ASSERT_THROW(store.freeBlock(key), logic_error);
}

/*
TEST_F(BlockStoreTest, FreeBlock) {
	auto key = store.allocBlock();
	store.freeBlock(key);
}

TEST_F(BlockStoreTest, FreeingDecreasesLength) {
	auto key1 = store.allocBlock();
	auto key2 = store.allocBlock();
	store.freeBlock(key1);
	ASSERT_EQ(1, store.length());
	store.freeBlock(key2);
	ASSERT_EQ(0, store.length());
}
*/

TEST_F(BlockStoreTest, Capacity) {
	store.capacity();
}

TEST_F(BlockStoreTest, AllocIncreasesCapacityIfNoSpaceIsAvailable) {
	auto initialCapacity = store.capacity();
	while (store.length() < store.capacity())
		store.allocBlock();

	store.allocBlock();
	ASSERT_LT(initialCapacity, store.capacity());
}

TEST_F(BlockStoreTest, SetCapacity) {
	auto newCapacity = store.capacity() + 3;
	store.setCapacity(newCapacity);
	ASSERT_LE(newCapacity, store.capacity());
}

TEST_F(BlockStoreTest, SettingCapacityBelowLengthThrows) {
	store.allocBlock();
	ASSERT_THROW(store.setCapacity(0), length_error);
}

class BlockStoreManagerTest : public ::testing::Test {
protected:
	~BlockStoreManagerTest() {
		BlockStoreManager::deleteAllInstances();
	}
};

TEST_F(BlockStoreManagerTest, Instance) {
	auto& store = BlockStoreManager::instance(64);
	ASSERT_EQ(64, store.blockSize());
}

TEST_F(BlockStoreManagerTest, SameInstanceIsReturnedForSameCapacity) {
	auto& store1 = BlockStoreManager::instance(24);
	auto& store2 = BlockStoreManager::instance(24);
	ASSERT_EQ(&store1, &store2);
}

TEST_F(BlockStoreManagerTest, InstancesAreIterable) {
	BlockStoreManager::instance(16);
	BlockStoreManager::instance(24);
	BlockStoreManager::instance(32);

	array<size_t, 3> blockSizes;
	int i{};
	for (IBlockStore& store : BlockStoreManager())
		blockSizes[i++] = store.blockSize();

	ASSERT_EQ(3, i);

	sort(blockSizes.begin(), blockSizes.end());
	ASSERT_EQ(16, blockSizes[0]);
	ASSERT_EQ(24, blockSizes[1]);
	ASSERT_EQ(32, blockSizes[2]);
}

TEST_F(BlockStoreManagerTest, CapacityCanBeReachedViaIterator) {
	BlockStoreManager::instance(16).allocBlock();
	IBlockStore& store = *BlockStoreManager::begin();
	auto& cap = dynamic_cast<ICapacityControl&>(store);
	ASSERT_LE(0, cap.capacity());
}

} // namespace
