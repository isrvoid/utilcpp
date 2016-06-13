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

// TODO hash

} // namespace
