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
    size_t length;
    Len::read(data, length);
}

TEST_F(LengthEncodingTest, ZeroedOutDataYieldsLength0) {
    size_t length;
    Len::read(data, length);
    ASSERT_EQ(0, length);
}

TEST_F(LengthEncodingTest, ReadAdvancesData) {
    size_t length;
    auto p = Len::read(data, length);
    ASSERT_EQ(data + 1, p);
}

TEST_F(LengthEncodingTest, ReadMaxByteValue) {
    data[0] = Len::byteLengthMax;
    size_t length;
    ASSERT_EQ(data + 1, Len::read(data, length));
    auto byteLengthMax = Len::byteLengthMax;
    ASSERT_EQ(byteLengthMax, length);
}

TEST_F(LengthEncodingTest, Write) {
    Len::write(0, data);
}

TEST_F(LengthEncodingTest, WriteBack) {
    Len::writeBack(0, dataEnd);
}

TEST_F(LengthEncodingTest, WriteLength0YieldsZeroedOutData) {
    Len::write(0, data);
    ASSERT_EQ(0, std::memcmp(init, data, sizeof(init)));
}

TEST_F(LengthEncodingTest, WriteReveseLength0YieldsZeroedOutData) {
    Len::writeBack(0, dataEnd);
    ASSERT_EQ(0, std::memcmp(init, data, sizeof(init)));
}

TEST_F(LengthEncodingTest, WriteAdvancesData) {
    ASSERT_EQ(data + 1, Len::write(0, data));
}

TEST_F(LengthEncodingTest, WriteBackDecreasesData) {
    ASSERT_EQ(dataEnd - 1, Len::writeBack(0, dataEnd));
}

TEST_F(LengthEncodingTest, WriteMaxByteValue) {
    ASSERT_EQ(data + 1, Len::write(Len::byteLengthMax, data));
    auto byteLengthMax = Len::byteLengthMax;
    ASSERT_EQ(byteLengthMax, data[0]);
    ASSERT_EQ(0, memcmp(init + 1, data + 1, sizeof(init) - 1));
}

TEST_F(LengthEncodingTest, WriteBackMaxByteValue) {
    ASSERT_EQ(dataEnd - 1, Len::writeBack(Len::byteLengthMax, dataEnd));
    auto byteLengthMax = Len::byteLengthMax;
    ASSERT_EQ(byteLengthMax, dataEnd[-1]);
    ASSERT_EQ(0, memcmp(init, data, sizeof(init) - 1));
}

TEST_F(LengthEncodingTest, WriteShortMinAdvancesDataBy2) {
    ASSERT_EQ(data + 2, Len::write(Len::byteLengthMax + 1, data));
}

TEST_F(LengthEncodingTest, WriteBackShortMinDecreasesDataBy2) {
    ASSERT_EQ(dataEnd - 2, Len::writeBack(Len::byteLengthMax + 1, dataEnd));
}

TEST_F(LengthEncodingTest, ReadShortMinAdvancesDataBy2) {
    Len::write(Len::byteLengthMax + 1, data);
    size_t length;
    ASSERT_EQ(data + 2, Len::read(data, length));
}

TEST_F(LengthEncodingTest, ShortMin) {
    auto length = Len::byteLengthMax + 1;
    ASSERT_EQ(data + 2, Len::write(length, data));
    size_t verify;
    ASSERT_EQ(data + 2, Len::read(data, verify));
    ASSERT_EQ(length, verify);

    ASSERT_EQ(0, memcmp(init + 2, data + 2, sizeof(init) - 2));
}

TEST_F(LengthEncodingTest, ShortMinBack) {
    auto length = Len::byteLengthMax + 1;
    ASSERT_EQ(dataEnd - 2, Len::writeBack(length, dataEnd));
    size_t verify;
    ASSERT_EQ(dataEnd, Len::read(dataEnd - 2, verify));
    ASSERT_EQ(length, verify);

    ASSERT_EQ(0, memcmp(init, data, sizeof(init) - 2));
}

TEST_F(LengthEncodingTest, ShortMax) {
    auto length = Len::shortLengthMax;
    ASSERT_EQ(data + 2, Len::write(length, data));
    size_t verify;
    ASSERT_EQ(data + 2, Len::read(data, verify));
    ASSERT_EQ(length, verify);

    ASSERT_EQ(0, memcmp(init + 2, data + 2, sizeof(init) - 2));
}

TEST_F(LengthEncodingTest, ShortMaxBack) {
    auto length = Len::shortLengthMax;
    ASSERT_EQ(dataEnd - 2, Len::writeBack(length, dataEnd));
    size_t verify;
    ASSERT_EQ(dataEnd, Len::read(dataEnd - 2, verify));
    ASSERT_EQ(length, verify);

    ASSERT_EQ(0, memcmp(init, data, sizeof(init) - 2));
}

TEST_F(LengthEncodingTest, LongMin) {
    auto length = Len::shortLengthMax + 1;
    ASSERT_EQ(dataEnd, Len::write(length, data));
    size_t verify;
    ASSERT_EQ(dataEnd, Len::read(data, verify));
    ASSERT_EQ(length, verify);
}

TEST_F(LengthEncodingTest, LongMinBack) {
    auto length = Len::shortLengthMax + 1;
    ASSERT_EQ(data, Len::writeBack(length, dataEnd));
    size_t verify;
    ASSERT_EQ(dataEnd, Len::read(data, verify));
    ASSERT_EQ(length, verify);
}

TEST_F(LengthEncodingTest, LongMax) {
    auto length = Len::lengthMax;
    ASSERT_EQ(dataEnd, Len::write(length, data));
    size_t verify;
    ASSERT_EQ(dataEnd, Len::read(data, verify));
    ASSERT_EQ(length, verify);
}

TEST_F(LengthEncodingTest, LongMaxBack) {
    auto length = Len::lengthMax;
    ASSERT_EQ(data, Len::writeBack(length, dataEnd));
    size_t verify;
    ASSERT_EQ(dataEnd, Len::read(data, verify));
    ASSERT_EQ(length, verify);
}

#define LENGTH_ENCODING_TEST_IMPL()  \
        size_t verify; \
        auto advancedData = Len::write(length, data); \
        ASSERT_EQ(advancedData, Len::read(data, verify)); \
        ASSERT_EQ(length, verify); \
 \
        memset(data, 0, sizeof(data)); \
        ASSERT_EQ(data, Len::writeBack(length, advancedData)); \
        ASSERT_EQ(advancedData, Len::read(data, verify)); \
        ASSERT_EQ(length, verify)

TEST_F(LengthEncodingTest, SmallValues) {
    for (size_t length = 0; length < (1 << 10); length += 0xa5) {
        LENGTH_ENCODING_TEST_IMPL();
    }
}

TEST_F(LengthEncodingTest, MediumValues) {
    for (size_t length = 0; length < (1 << 18); length += 0xaa55) {
        LENGTH_ENCODING_TEST_IMPL();
    }
}

TEST_F(LengthEncodingTest, LargeValues) {
    for (size_t length = Len::lengthMax, i = 0; i < 8; length -= 0xaaa5555, i++) {
        LENGTH_ENCODING_TEST_IMPL();
    }
}

} // namespace
