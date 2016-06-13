#include <gtest/gtest.h>

#include <cstring>

#include <util/sha1/digest.h>

namespace {

using util::sha1::hash;
using util::sha1::Digest;
#include "sha1testvectors.cpp"

class UtilSHA1Test : public ::testing::Test {
protected:
	Digest sha;
};

TEST_F(UtilSHA1Test, EmptyInput) {
	ASSERT_EQ(outputEmpty, sha.finish());
}

TEST_F(UtilSHA1Test, EmptyInput2) {
	sha.put(nullptr, 0);
	sha.put(nullptr, 0);
	ASSERT_EQ(outputEmpty, sha.finish());
}

TEST_F(UtilSHA1Test, EmptyInput3) {
	ASSERT_EQ(outputEmpty, sha.digest(nullptr, 0));
}

TEST_F(UtilSHA1Test, SimpleInput) {
	ASSERT_EQ(output1, sha.digest(input1, input1Length));
}

TEST_F(UtilSHA1Test, SingleBytePut) {
	for (size_t i = 0; i < input2Length; ++i)
		sha.put(input2[i]);

	ASSERT_EQ(output2, sha.finish());
}

TEST_F(UtilSHA1Test, StartResetsState) {
	sha.put("hi", 2);
	sha.start();
	sha.put(nullptr, 0);
	ASSERT_EQ(outputEmpty, sha.finish());
}

TEST_F(UtilSHA1Test, DigestMethodDoesntRequireStart) {
	sha.put("foo", 3);
	ASSERT_EQ(output1, sha.digest(input1, input1Length));
	ASSERT_EQ(output2, sha.digest(input2, input2Length));
}

TEST_F(UtilSHA1Test, StateIsDirtyAfterResult) {
	ASSERT_NE(sha.finish(), sha.finish());
	sha.start();
	auto res = sha.digest(nullptr, 0);
	ASSERT_NE(res, sha.finish());
}

TEST_F(UtilSHA1Test, FurtherInputs) {
	ASSERT_EQ(output3, sha.digest(input3, input3Length));
	ASSERT_EQ(output4, sha.digest(input4, input4Length));
}

TEST_F(UtilSHA1Test, ChunkInput) {
	uint32_t remaining = input3Length;
	uint32_t chunkLengthHelper = 4;
	auto* p = input3;
	while (remaining) {
		uint32_t chunkLength = chunkLengthHelper % 23;
		chunkLength = chunkLength <= remaining ? chunkLength : remaining;
		sha.put(p, chunkLength);
		p += chunkLength;
		remaining -= chunkLength;
		--chunkLengthHelper;
	}
	ASSERT_EQ(output3, sha.finish());
}

/*
// takes only few ms on PC but would take longer on the target (< 0.2 s)
TEST_F(UtilSHA1Test, LongMillionA) {
	constexpr uint8_t a[] = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";

	for (int i = 0; i < 25'000; ++i)
		sha.put(a, 40);

	ASSERT_EQ(outputMillionA, sha.finish());
}
*/

} // namespace
