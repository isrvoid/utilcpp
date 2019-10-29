#include <gtest/gtest.h>

#include <util/serdeimpl.h>

using namespace std;
using namespace util::serde;

namespace {

class SerdeVectorTest : public testing::Test {
protected:
    vector<uint8_t> buf;
    shared_ptr<vector<int>> ptr;
};

TEST_F(SerdeVectorTest, Init) {
    buf.reserve(64);
    vector<int> init;
    auto dest = serializevector<int>(&init, buf.data(), buf.data() + buf.capacity());
    auto const src = deserializevector<int>(buf.data(), buf.data() + buf.capacity(), &ptr);
    ASSERT_NE(nullptr, dest);
    ASSERT_EQ(dest, src);
    ASSERT_EQ(init, *ptr);
}

TEST_F(SerdeVectorTest, SingleElement) {
    buf.reserve(64);
    vector<int> v{42};
    auto dest = serializevector<int>(&v, buf.data(), buf.data() + buf.capacity());
    auto const src = deserializevector<int>(buf.data(), buf.data() + buf.capacity(), &ptr);
    ASSERT_EQ(dest, src);
    ASSERT_EQ(v, *ptr);
}

TEST_F(SerdeVectorTest, MultipleElements) {
    buf.reserve(64);
    vector<int> v{42, 0, -42};
    auto dest = serializevector<int>(&v, buf.data(), buf.data() + buf.capacity());
    auto const src = deserializevector<int>(buf.data(), buf.data() + buf.capacity(), &ptr);
    ASSERT_EQ(dest, src);
    ASSERT_EQ(v, *ptr);
}

TEST_F(SerdeVectorTest, ByteVector) {
    buf.reserve(64);
    vector<uint8_t> v{0xfd, 0xfe, 0xff};
    auto dest = serializevector<uint8_t>(&v, buf.data(), buf.data() + buf.capacity());
    shared_ptr<vector<uint8_t>> bytePtr;
    auto const src = deserializevector<uint8_t>(buf.data(), buf.data() + buf.capacity(), &bytePtr);
    ASSERT_EQ(dest, src);
    ASSERT_EQ(v, *bytePtr);
}

TEST_F(SerdeVectorTest, SerBufferTooShort) {
    buf.reserve(64);
    vector<int> v{0x5a5a5a5a, 0x5f5f5f5f, 0x55555555};
    auto dest = serializevector<int>(&v, buf.data(), buf.data() + buf.capacity());
    ASSERT_EQ(nullptr, serializevector<int>(&v, buf.data(), static_cast<uint8_t*>(dest) - 1));
}

TEST_F(SerdeVectorTest, DeBufferTooShort) {
    buf.reserve(64);
    vector<int> v{0x5a5a5a5a, 0x5f5f5f5f, 0x55555555};
    auto dest = serializevector<int>(&v, buf.data(), buf.data() + buf.capacity());
    ASSERT_EQ(nullptr, deserializevector<int>(buf.data(), static_cast<uint8_t*>(dest) - 1, &ptr));
}

} // namespace
