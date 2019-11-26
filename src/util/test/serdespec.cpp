#include <gtest/gtest.h>

#include <util/serdespecbasic.h>
#include <util/serdespec.h>

using namespace std;
using namespace util::serde;

namespace {

TEST(SerdeSpecTest, IsTrivial) {
    ASSERT_TRUE(isTrivialSerde<int>());
    ASSERT_FALSE(isTrivialSerde<string>());
}

TEST(SerdeSpecTest, Int) {
    constexpr SerdePair expect{nullptr, nullptr};
    ASSERT_EQ(expect, serdePair<int>());
}

TEST(TypeSpecTest, ByteVector) {
    constexpr SerdePair expect{serializevector<uint8_t>, deserializevector<uint8_t>};
    ASSERT_EQ(expect, serdePair<vector<uint8_t>>());
}

TEST(TypeSpecTest, String) {
    constexpr SerdePair expect{serializestring, deserializestring};
    ASSERT_EQ(expect, serdePair<string>());
}

TEST(TypeSpecTest, StringVector) {
    constexpr SerdePair expect{serializevector<string>, deserializevector<string>};
    ASSERT_EQ(expect, serdePair<vector<string>>());
}

} // namespace
