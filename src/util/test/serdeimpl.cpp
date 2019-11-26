#include <gtest/gtest.h>

#include <util/serdespecbasic.h>
#include <util/serdeimpl.h>

using namespace std;
using namespace util::serde;

namespace {

class SerdeVectorTest : public testing::Test {
protected:
    vector<uint8_t> buf = vector<uint8_t>(64);
    shared_ptr<vector<int>> ptr;
};

TEST_F(SerdeVectorTest, Init) {
    vector<int> init;
    auto dest = serializevector<int>(&init, buf.data(), buf.data() + buf.capacity());
    auto const src = deserializevector<int>(buf.data(), buf.data() + buf.capacity(), &ptr);
    ASSERT_NE(nullptr, dest);
    ASSERT_EQ(dest, src);
    ASSERT_EQ(init, *ptr);
}

TEST_F(SerdeVectorTest, SingleElement) {
    vector<int> v{42};
    auto dest = serializevector<int>(&v, buf.data(), buf.data() + buf.capacity());
    auto const src = deserializevector<int>(buf.data(), buf.data() + buf.capacity(), &ptr);
    ASSERT_EQ(dest, src);
    ASSERT_EQ(v, *ptr);
}

TEST_F(SerdeVectorTest, MultipleElements) {
    vector<int> v{42, 0, -42};
    auto dest = serializevector<int>(&v, buf.data(), buf.data() + buf.capacity());
    auto const src = deserializevector<int>(buf.data(), buf.data() + buf.capacity(), &ptr);
    ASSERT_EQ(dest, src);
    ASSERT_EQ(v, *ptr);
}

TEST_F(SerdeVectorTest, ByteVector) {
    vector<uint8_t> v{0xfd, 0xfe, 0xff};
    auto dest = serializevector<uint8_t>(&v, buf.data(), buf.data() + buf.capacity());
    shared_ptr<vector<uint8_t>> bytePtr;
    auto const src = deserializevector<uint8_t>(buf.data(), buf.data() + buf.capacity(), &bytePtr);
    ASSERT_EQ(dest, src);
    ASSERT_EQ(v, *bytePtr);
}

TEST_F(SerdeVectorTest, SerBufferTooShort) {
    vector<int> v{0x5a5a5a5a, 0x5f5f5f5f, 0x55555555};
    auto dest = serializevector<int>(&v, buf.data(), buf.data() + buf.capacity());
    ASSERT_EQ(nullptr, serializevector<int>(&v, buf.data(), static_cast<uint8_t*>(dest) - 1));
}

TEST_F(SerdeVectorTest, DeserBufferTooShort) {
    vector<int> v{0x5a5a5a5a, 0x5f5f5f5f, 0x55555555};
    auto dest = serializevector<int>(&v, buf.data(), buf.data() + buf.capacity());
    ASSERT_EQ(nullptr, deserializevector<int>(buf.data(), static_cast<uint8_t*>(dest) - 1, &ptr));
}

class SerdeStringTest : public testing::Test {
protected:
    vector<uint8_t> buf = vector<uint8_t>(64);
    shared_ptr<string> ptr;
};

TEST_F(SerdeStringTest, Init) {
    const string init;
    auto dest = serializestring(&init, buf.data(), buf.data() + buf.capacity());
    auto const src = deserializestring(buf.data(), buf.data() + buf.capacity(), &ptr);
    ASSERT_NE(nullptr, dest);
    ASSERT_EQ(dest, src);
    ASSERT_EQ(init, *ptr);
}

TEST_F(SerdeStringTest, SingleChar) {
    string val{"x"};
    auto dest = serializestring(&val, buf.data(), buf.data() + buf.capacity());
    auto const src = deserializestring(buf.data(), buf.data() + buf.capacity(), &ptr);
    ASSERT_EQ(dest, src);
    ASSERT_EQ(val, *ptr);
}

TEST_F(SerdeStringTest, MultipleChars) {
    string val{"The quick brown fox\n"};
    auto dest = serializestring(&val, buf.data(), buf.data() + buf.capacity());
    auto const src = deserializestring(buf.data(), buf.data() + buf.capacity(), &ptr);
    ASSERT_EQ(dest, src);
    ASSERT_EQ(val, *ptr);
}

TEST_F(SerdeStringTest, SerBufferTooShort) {
    string val{"foo"};
    auto dest = serializestring(&val, buf.data(), buf.data() + buf.capacity());
    ASSERT_EQ(nullptr, serializestring(&val, buf.data(), static_cast<uint8_t*>(dest) - 1));
}

TEST_F(SerdeStringTest, DeserBufferTooShort) {
    string val{"foo"};
    auto dest = serializestring(&val, buf.data(), buf.data() + buf.capacity());
    ASSERT_EQ(nullptr, deserializestring(buf.data(), static_cast<uint8_t*>(dest) - 1, &ptr));
}

class NonTrivialSerdeVectorTest : public testing::Test {
protected:
    vector<uint8_t> buf = vector<uint8_t>(128);
    shared_ptr<vector<string>> ptr;
};

TEST_F(NonTrivialSerdeVectorTest, Init) {
    vector<string> init;
    auto dest = serializevector<int>(&init, buf.data(), buf.data() + buf.capacity());
    auto const src = deserializevector<int>(buf.data(), buf.data() + buf.capacity(), &ptr);
    ASSERT_NE(nullptr, dest);
    ASSERT_EQ(dest, src);
    ASSERT_EQ(init, *ptr);
}

TEST_F(NonTrivialSerdeVectorTest, SingleElement) {
    vector<string> v{"foo"};
    auto dest = serializevector<int>(&v, buf.data(), buf.data() + buf.capacity());
    auto const src = deserializevector<int>(buf.data(), buf.data() + buf.capacity(), &ptr);
    ASSERT_EQ(dest, src);
    ASSERT_EQ(v, *ptr);
}

TEST_F(NonTrivialSerdeVectorTest, MultipleElements) {
    vector<string> v{"foo", "bar", "The quick brown fox"};
    auto dest = serializevector<string>(&v, buf.data(), buf.data() + buf.capacity());
    auto const src = deserializevector<string>(buf.data(), buf.data() + buf.capacity(), &ptr);
    ASSERT_EQ(dest, src);
    ASSERT_EQ(v, *ptr);
}

TEST_F(NonTrivialSerdeVectorTest, SerBufferTooShort) {
    vector<string> v{"The quick", "brown fox"};
    auto dest = serializevector<string>(&v, buf.data(), buf.data() + buf.capacity());
    ASSERT_EQ(nullptr, serializevector<string>(&v, buf.data(), static_cast<uint8_t*>(dest) - 1));
}

TEST_F(NonTrivialSerdeVectorTest, DeserBufferTooShort) {
    vector<string> v{"jumps over", "the lazy", "dog"};
    auto dest = serializevector<string>(&v, buf.data(), buf.data() + buf.capacity());
    ASSERT_EQ(nullptr, deserializevector<string>(buf.data(), static_cast<uint8_t*>(dest) - 1, &ptr));
}

} // namespace
