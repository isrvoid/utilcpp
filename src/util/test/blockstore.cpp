#include <gtest/gtest.h>

#include <stdio.h>
#include <memory>
#include <atomic>
#include <cstring>

#include <util/blockstore.h>

using namespace std;

namespace util {

class FakeBlockGuardTest : public ::testing::Test {
protected:
	unique_ptr<BlockGuard> block0 = make_unique<FakeBlockGuard<0>>();
	unique_ptr<BlockGuard> block1 = make_unique<FakeBlockGuard<1>>();
	unique_ptr<BlockGuard> block2 = make_unique<FakeBlockGuard<2>>();
	unique_ptr<BlockGuard> block4 = make_unique<FakeBlockGuard<4>>();
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
	const uint32_t store = 0xa555aaa5;
	uint32_t load;
	block4->store(&store);
	block4->load(&load);
	ASSERT_EQ(store, load);
}

TEST(MaxAtomicTest, AssignmentAffectsAllBytes) {
	// more of a sanity check
	MaxAtomic expected, v;
	memset(&expected, 0x5a, sizeof(expected));
	v = expected;
	ASSERT_EQ(0, memcmp(&expected, &v, sizeof(expected)));
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

// platform dependent
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

template<typename T>
bool operator==(const T& lhs, const T& rhs) {
	return !memcmp(&lhs, &rhs, sizeof(lhs));
}

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

TEST_F(AtomicBlockStoreTest, LoadThrowsAtInvalidKey) {
	MaxAtomic val;
	ASSERT_THROW(store.load(0, &val), out_of_range);
	ASSERT_THROW(store.load(42, &val), out_of_range);
}

TEST_F(AtomicBlockStoreTest, StoreThrowsAtInvalidKey) {
	MaxAtomic val;
	ASSERT_THROW(store.store(0, &val), out_of_range);
	ASSERT_THROW(store.store(42, &val), out_of_range);
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

TEST_F(AtomicBlockStoreTest, CapacityIsNonZeroAfterAllocation) {
	store.allocBlock();
	ASSERT_LT(0, store.capacity());
}

TEST_F(AtomicBlockStoreTest, SetCapacity) {
	ASSERT_GT(100, store.capacity());
	store.setCapacity(100);
	ASSERT_LE(100, store.capacity());
}

TEST_F(AtomicBlockStoreTest, SettingCapacityBelowLengthThrows) {
	store.allocBlock();
	ASSERT_THROW(store.setCapacity(0), length_error);
}

TEST(BlockStoreManager, GetStore) {
	// FIXME
	//auto& store = BlockStoreManager::instance(64);
	//ASSERT_EQ(64, store.blockSize());
}

} // namespace
