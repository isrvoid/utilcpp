#include <gtest/gtest.h>

#include <stdio.h>
#include <atomic>

#include <util/blockstore.h>

using namespace std;

namespace util {

class FakeBlockGuardTest : public ::testing::Test {
protected:
	unique_ptr<BlockGuard> block1 = make_unique<FakeBlockGuard<1>>();
	unique_ptr<BlockGuard> block2 = make_unique<FakeBlockGuard<2>>();
	unique_ptr<BlockGuard> block4 = make_unique<FakeBlockGuard<4>>();
};

TEST_F(FakeBlockGuardTest, BlockSize1) {
	ASSERT_EQ(1, block1->blockSize());
}

TEST_F(FakeBlockGuardTest, BlockSize2) {
	ASSERT_EQ(2, block2->blockSize());
}

TEST_F(FakeBlockGuardTest, BlockSize4) {
	ASSERT_EQ(4, block4->blockSize());
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

class AtomicBlockStoreTest : public ::testing::Test {
protected:
	AtomicBlockStore store;
};

TEST_F(AtomicBlockStoreTest, BlockSize) {
	store.blockSize();
}

// platform dependent -- least common denominator between 32 bit ARM and x86_64
TEST_F(AtomicBlockStoreTest, BlockSizeIsAtomic) {
	ASSERT_EQ(8, store.blockSize());
	atomic<double> v1;

	struct Foo {
		int32_t x, y;
	};
	atomic<Foo> v2;

	ASSERT_TRUE(atomic_is_lock_free(&v1));
	ASSERT_TRUE(atomic_is_lock_free(&v2));
}

TEST_F(AtomicBlockStoreTest, TakeBlock) {
	auto key = store.takeBlock();
}

} // namespace
