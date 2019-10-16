#include <gtest/gtest.h>

#include <memory>

#include <util/typeinfo.h>
#include <util/typeinfofactory.h>
#include <util/digest/crc.h>

using namespace std;
using namespace util;

struct NonPolyBase {};
struct NonPolyDerived : NonPolyBase {};

struct PolyBase { virtual ~PolyBase() = default; virtual void foo() {} };
struct PolyDerived : PolyBase {};

namespace {

TEST(TypeNameTest, Int) {
    ASSERT_STREQ("int", typeName(42));
}

TEST(TypeNameTest, IsConstexpr) {
    constexpr auto name = typeName(42u);
    ASSERT_STREQ("unsigned int", name);
}

TEST(TypeNameTest, TypedefIsErased) {
    ASSERT_STREQ("unsigned char", typeName(static_cast<uint8_t>(42)));
}

TEST(TypeNameTest, SharedPtr) {
    ASSERT_STREQ("std::shared_ptr<int>", typeName(make_shared<int>(42)));
}

TEST(TypeNameTest, NestedTypes) {
    ASSERT_STREQ("std::shared_ptr<std::vector<bool>>", typeName(make_shared<vector<bool>>()));

    ASSERT_STREQ(("std::vector<std::vector<std::pair<int, bool>>>"),
            typeName(std::vector<std::vector<std::pair<int, bool>>>{}));
}

TEST(TypeNameTest, Void) {
    ASSERT_STREQ("void", typeName<void>());
}

TEST(TypeNameTest, IsNotPolymorphic) {
    NonPolyDerived d1;
    NonPolyBase& b1 = d1;
    ASSERT_STREQ("NonPolyBase", typeName(b1));

    PolyDerived d2;
    PolyBase& b2 = d2;
    ASSERT_STREQ("PolyBase", typeName(b2));
}

constexpr uint32_t crcOf(const char* s) {
    digest::CRC32 crc;
    while (*s != '\0')
        crc.put(static_cast<uint8_t>(*s++));

    return crc.finish();
}

TEST(TypeHashTest, IsCrcOverName) {
    int i = 42;
    ASSERT_EQ(crcOf(typeName(i)), typeHash(i));

    std::vector<std::vector<std::pair<int, bool>>> v;
    ASSERT_EQ(crcOf(typeName(v)), typeHash(v));
}

TEST(TypeHashTest, IsConstexpr) {
    constexpr uint32_t hash = typeHash(42u);
    ASSERT_EQ(crcOf("unsigned int"), hash);
}

TEST(TypeInfoFactoryTest, Int) {
    const auto expect = TypeInfo{typeHash<int>(), sizeof(int), nullptr};
    ASSERT_EQ(0, memcmp(&expect, &TypeInfoFactory<int>::create(), sizeof(expect)));
}

TEST(TypeInfoFactoryTest, ByteVector) {
    const auto expectSerialize = serde::serializevector<uint8_t>;
    const auto expectDeserialize = serde::deserializevector<uint8_t>;

    const auto res = TypeInfoFactory<vector<uint8_t>>::create();
    ASSERT_EQ(res.hash, typeHash<vector<uint8_t>>());
    ASSERT_EQ(res.size, sizeof(vector<uint8_t>));
    const auto serde = static_cast<const pair<serde::serializeFun, serde::deserializeFun>*>(res.serde);
    ASSERT_EQ(expectSerialize, serde->first);
    ASSERT_EQ(expectDeserialize, serde->second);
}

} // namespace

