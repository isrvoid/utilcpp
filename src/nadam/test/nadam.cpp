#include <gtest/gtest.h>

#include <nadam/types.h>

using namespace nadam;

namespace {

TEST(NadamMessageInfoTest, Create) {
	MessageInfo mi{"foo", 42, true};
	ASSERT_EQ(42, mi.maxSize);
	ASSERT_TRUE(mi.isVariableSize);
}

TEST(NadamMessageInfoTest, FixedSizeIsDefault) {
	MessageInfo mi{"foo", 42};
	ASSERT_FALSE(mi.isVariableSize);
}

TEST(NadamMessageInfoTest, SizeIdentifiersAreAliases) {
	MessageInfo mi{"foo", 42};
	ASSERT_EQ(42, mi.size);
	ASSERT_EQ(42, mi.maxSize);
}

TEST(NadamMessageInfoTest, ConstructHash) {
	MessageInfo mi{"foo", 0x5a, true};

	uint8_t expectedInput[] = {'f', 'o', 'o', 1, 0x0, 0x0, 0x0, 0x5a};

	util::digest::SHA1 sha;
	ASSERT_EQ(sha.digest(expectedInput, sizeof(expectedInput)), mi.hash);
}

} // namespace
