#include <gtest/gtest.h>

#include <util/blockstore.h>

namespace util {

class FakeBlockGuardTest : public ::testing::Test {
protected:
	std::unique_ptr<BlockGuard> block1 = std::make_unique<FakeBlockGuard<1>>();
	std::unique_ptr<BlockGuard> block2 = std::make_unique<FakeBlockGuard<2>>();
	std::unique_ptr<BlockGuard> block4 = std::make_unique<FakeBlockGuard<4>>();
};

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

} // namespace
