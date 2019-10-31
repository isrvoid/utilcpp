#include <gtest/gtest.h>

#include <util/serdespec.h>

using namespace std;
using namespace util;

namespace {

TEST(SerdeSpecTest, Int) {
    constexpr auto serde = util::serde::serdePair<int>();
    ASSERT_EQ(nullptr, serde.first);
    ASSERT_EQ(nullptr, serde.second);
}

TEST(TypeSpecTest, ByteVector) {
    const auto expectSerialize = serde::serializevector<uint8_t>;
    const auto expectDeserialize = serde::deserializevector<uint8_t>;

    constexpr auto serde = serde::serdePair<vector<uint8_t>>();
    ASSERT_EQ(expectSerialize, serde.first);
    ASSERT_EQ(expectDeserialize, serde.second);
}

} // namespace
