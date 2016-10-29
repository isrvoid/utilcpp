#include <gtest/gtest.h>

#include <cstring>

#include <util/lengthencoding.h>

namespace {

using Len = ::util::LengthEncoding;

class LengthEncodingTest : public ::testing::Test {
protected:
	uint8_t data[Len::byteCountMax]{};
	uint8_t* const dataEnd = data + Len::byteCountMax;
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

TEST_F(LengthEncodingTest, WriteReverse) {
	Len::writeReverse(0, dataEnd);
}

TEST_F(LengthEncodingTest, WriteLength0YieldsZeroedOutData) {
	Len::write(0, data);
	ASSERT_EQ(0, std::memcmp(init, data, sizeof(init)));
}

TEST_F(LengthEncodingTest, WriteReveseLength0YieldsZeroedOutData) {
	Len::writeReverse(0, dataEnd);
	ASSERT_EQ(0, std::memcmp(init, data, sizeof(init)));
}

TEST_F(LengthEncodingTest, WriteAdvancesData) {
	ASSERT_EQ(data + 1, Len::write(0, data));
}

TEST_F(LengthEncodingTest, WriteReverseDecreasesData) {
	ASSERT_EQ(dataEnd - 1, Len::writeReverse(0, dataEnd));
}

TEST_F(LengthEncodingTest, WriteMaxByteValue) {
	ASSERT_EQ(data + 1, Len::write(Len::byteLengthMax, data));
	auto byteLengthMax = Len::byteLengthMax;
	ASSERT_EQ(byteLengthMax, data[0]);
	ASSERT_EQ(0, memcmp(init + 1, data + 1, sizeof(init) - 1));
}

TEST_F(LengthEncodingTest, WriteReverseMaxByteValue) {
	ASSERT_EQ(dataEnd - 1, Len::writeReverse(Len::byteLengthMax, dataEnd));
	auto byteLengthMax = Len::byteLengthMax;
	ASSERT_EQ(byteLengthMax, dataEnd[-1]);
	ASSERT_EQ(0, memcmp(init, data, sizeof(init) - 1));
}

TEST_F(LengthEncodingTest, WriteShortMinAdvancesDataBy2) {
	ASSERT_EQ(data + 2, Len::write(Len::byteLengthMax + 1, data));
}

TEST_F(LengthEncodingTest, WriteReverseShortMinDecreasesDataBy2) {
	ASSERT_EQ(dataEnd - 2, Len::writeReverse(Len::byteLengthMax + 1, dataEnd));
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

TEST_F(LengthEncodingTest, ShortMinReverse) {
	auto length = Len::byteLengthMax + 1;
	ASSERT_EQ(dataEnd - 2, Len::writeReverse(length, dataEnd));
	uint64_t verify;
	ASSERT_EQ(dataEnd, Len::read(dataEnd - 2, &verify));
	ASSERT_EQ(length, verify);

	ASSERT_EQ(0, memcmp(init, data, sizeof(init) - 2));
}

TEST_F(LengthEncodingTest, ShortMax) {
	auto length = Len::shortLengthMax;
	ASSERT_EQ(data + 2, Len::write(length, data));
	uint64_t verify;
	ASSERT_EQ(data + 2, Len::read(data, &verify));
	ASSERT_EQ(length, verify);

	ASSERT_EQ(0, memcmp(init + 2, data + 2, sizeof(init) - 2));
}

TEST_F(LengthEncodingTest, ShortMaxReverse) {
	auto length = Len::shortLengthMax;
	ASSERT_EQ(dataEnd - 2, Len::writeReverse(length, dataEnd));
	uint64_t verify;
	ASSERT_EQ(dataEnd, Len::read(dataEnd - 2, &verify));
	ASSERT_EQ(length, verify);

	ASSERT_EQ(0, memcmp(init, data, sizeof(init) - 2));
}

TEST_F(LengthEncodingTest, LongMin) {
	auto length = Len::shortLengthMax + 1;
	ASSERT_EQ(dataEnd, Len::write(length, data));
	uint64_t verify;
	ASSERT_EQ(dataEnd, Len::read(data, &verify));
	ASSERT_EQ(length, verify);
}

TEST_F(LengthEncodingTest, LongMinReverse) {
	auto length = Len::shortLengthMax + 1;
	ASSERT_EQ(data, Len::writeReverse(length, dataEnd));
	uint64_t verify;
	ASSERT_EQ(dataEnd, Len::read(data, &verify));
	ASSERT_EQ(length, verify);
}

TEST_F(LengthEncodingTest, LongMax) {
	auto length = Len::lengthMax;
	ASSERT_EQ(dataEnd, Len::write(length, data));
	uint64_t verify;
	ASSERT_EQ(dataEnd, Len::read(data, &verify));
	ASSERT_EQ(length, verify);
}

TEST_F(LengthEncodingTest, LongMaxReverse) {
	auto length = Len::lengthMax;
	ASSERT_EQ(data, Len::writeReverse(length, dataEnd));
	uint64_t verify;
	ASSERT_EQ(dataEnd, Len::read(data, &verify));
	ASSERT_EQ(length, verify);
}

#define LENGTH_ENCODING_TEST_IMPL()  \
		uint64_t verify; \
		auto advancedData = Len::write(length, data); \
		ASSERT_EQ(advancedData, Len::read(data, &verify)); \
		ASSERT_EQ(length, verify); \
 \
		memset(data, 0, sizeof(data)); \
		ASSERT_EQ(data, Len::writeReverse(length, advancedData)); \
		ASSERT_EQ(advancedData, Len::read(data, &verify)); \
		ASSERT_EQ(length, verify)

TEST_F(LengthEncodingTest, SmallValues) {
	for (unsigned int length = 0; length < (1 << 10); length += 0xa5) {
		LENGTH_ENCODING_TEST_IMPL();
	}
}

TEST_F(LengthEncodingTest, MediumValues) {
	for (unsigned int length = 0; length < (1 << 18); length += 0xaa55) {
		LENGTH_ENCODING_TEST_IMPL();
	}
}

TEST_F(LengthEncodingTest, LargeValues) {
	for (uint64_t length = 0; length < (static_cast<uint64_t>(1) << 34); length += 0xaaaa5555) {
		LENGTH_ENCODING_TEST_IMPL();
	}
}

} // namespace