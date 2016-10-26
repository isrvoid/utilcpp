#include <gtest/gtest.h>

#include <cstring>

#include <util/lengthencoding.h>

namespace {

using Len = ::util::LengthEncoding;

class LengthEncodingTest : public ::testing::Test {
protected:
	uint8_t data[Len::byteCountMax]{};
	const uint8_t init[Len::byteCountMax]{};
};

TEST_F(LengthEncodingTest, Read) {
	uint64_t length;
	Len::read(data, &length);
}

TEST_F(LengthEncodingTest, ZeroedOutDataYieldsLength0) {
	uint64_t length;
	Len::read(data, &length);
	ASSERT_EQ(0, length);
}

TEST_F(LengthEncodingTest, ReadAdvancesData) {
	uint64_t length;
	auto p = Len::read(data, &length);
	ASSERT_EQ(data + 1, p);
}

TEST_F(LengthEncodingTest, ReadMaxByteValue) {
	data[0] = Len::byteLengthMax;
	uint64_t length;
	ASSERT_EQ(data + 1, Len::read(data, &length));
	auto byteLengthMax = Len::byteLengthMax;
	ASSERT_EQ(byteLengthMax, length);
}

TEST_F(LengthEncodingTest, Write) {
	Len::write(0, data);
}

TEST_F(LengthEncodingTest, WriteLength0YieldsZeroedOutData) {
	Len::write(0, data);
	ASSERT_EQ(0, std::memcmp(init, data, sizeof(init)));
}

TEST_F(LengthEncodingTest, WriteAdvancesData) {
	ASSERT_EQ(data + 1, Len::write(0, data));
}

TEST_F(LengthEncodingTest, WriteMaxByteValue) {
	ASSERT_EQ(data + 1, Len::write(Len::byteLengthMax, data));
	auto byteLengthMax = Len::byteLengthMax;
	ASSERT_EQ(byteLengthMax, data[0]);
	ASSERT_EQ(0, memcmp(init + 1, data + 1, sizeof(init) - 1));
}

TEST_F(LengthEncodingTest, WriteShortMinAdvancesDataBy2) {
	ASSERT_EQ(data + 2, Len::write(Len::byteLengthMax + 1, data));
}

TEST_F(LengthEncodingTest, ReadShortMinAdvancesDataBy2) {
	Len::write(Len::byteLengthMax + 1, data);
	uint64_t length;
	ASSERT_EQ(data + 2, Len::read(data, &length));
}

TEST_F(LengthEncodingTest, ShortMin) {
	auto length = Len::byteLengthMax + 1;
	ASSERT_EQ(data + 2, Len::write(length, data));
	uint64_t verify;
	ASSERT_EQ(data + 2, Len::read(data, &verify));
	ASSERT_EQ(length, verify);
	ASSERT_EQ(0, memcmp(init + 2, data + 2, sizeof(init) - 2));
}

TEST_F(LengthEncodingTest, ShortMax) {
	auto length = Len::shortLengthMax;
	ASSERT_EQ(data + 2, Len::write(length, data));
	uint64_t verify;
	ASSERT_EQ(data + 2, Len::read(data, &verify));
	ASSERT_EQ(length, verify);
	ASSERT_EQ(0, memcmp(init + 2, data + 2, sizeof(init) - 2));
}

TEST_F(LengthEncodingTest, LongMin) {
	auto length = Len::shortLengthMax + 1;
	ASSERT_EQ(data + 8, Len::write(length, data));
	uint64_t verify;
	ASSERT_EQ(data + 8, Len::read(data, &verify));
	ASSERT_EQ(length, verify);
}

TEST_F(LengthEncodingTest, LongMax) {
	auto length = Len::lengthMax;
	ASSERT_EQ(data + 8, Len::write(length, data));
	uint64_t verify;
	ASSERT_EQ(data + 8, Len::read(data, &verify));
	ASSERT_EQ(length, verify);
}

TEST_F(LengthEncodingTest, SmallValues) {
	for (unsigned int length = 0; length < (1 << 10); length += 0xa5) {
		auto advancedData = Len::write(length, data);
		uint64_t verify;
		ASSERT_EQ(advancedData, Len::read(data, &verify));
		ASSERT_EQ(length, verify);
	}
}

TEST_F(LengthEncodingTest, MediumValues) {
	for (unsigned int length = 0; length < (1 << 18); length += 0xaa55) {
		auto advancedData = Len::write(length, data);
		uint64_t verify;
		ASSERT_EQ(advancedData, Len::read(data, &verify));
		ASSERT_EQ(length, verify);
	}
}

TEST_F(LengthEncodingTest, LargeValues) {
	for (uint64_t length = 0; length < (static_cast<uint64_t>(1) << 34); length += 0xaaaa5555) {
		auto advancedData = Len::write(length, data);
		uint64_t verify;
		ASSERT_EQ(advancedData, Len::read(data, &verify));
		ASSERT_EQ(length, verify);
	}
}

} // namespace
