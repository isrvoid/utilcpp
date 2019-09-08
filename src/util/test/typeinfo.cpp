#include <gtest/gtest.h>

#include <memory>

#include <util/typeinfo.h>
#include <util/digest/crc.h>

using namespace std;
using namespace util;

struct NonPolyBase {};
struct NonPolyDerived : NonPolyBase {};

struct PolyBase { virtual void foo() {} };
struct PolyDerived : PolyBase {};

namespace {

constexpr uint32_t crcOf(const char* s) {
    digest::CRC32 crc;
    while (*s != '\0')
        crc.put(static_cast<uint8_t>(*s++));

    return crc.finish();
}

// FIXME streq with typeName directly
TEST(TypeNameTest, Int) {
    ASSERT_EQ(crcOf("int"), typeHash(42));
}

TEST(TypeNameTest, IsConstexpr) {
    constexpr auto hash = typeHash(42u);
    ASSERT_EQ(crcOf("unsigned int"), hash);
}

TEST(TypeNameTest, TypedefIsErased) {
    ASSERT_EQ(crcOf("unsigned char"), typeHash(static_cast<uint8_t>(42)));
}

TEST(TypeNameTest, SharedPtr) {
    constexpr auto expect = crcOf("std::shared_ptr<int>");
    ASSERT_EQ(expect, typeHash(make_shared<int>(42)));
}

TEST(TypeNameTest, NestedTypes) {
    uint32_t expect;
    expect = crcOf("std::shared_ptr<std::vector<bool>>");
    ASSERT_EQ(expect, typeHash(make_shared<vector<bool>>()));

    expect = crcOf("std::vector<std::vector<std::pair<int, bool>>>");
    ASSERT_EQ(expect, typeHash(std::vector<std::vector<std::pair<int, bool>>>{}));
}

TEST(TypeNameTest, Void) {
    ASSERT_EQ(crcOf("void"), TypeInfoFactory<void>::hash());
}

TEST(TypeNameTest, IsNotPolymorphic) {
    NonPolyDerived d1;
    NonPolyBase& b1 = d1;
    ASSERT_EQ(crcOf("NonPolyBase"), typeHash(b1));

    PolyDerived d2;
    PolyBase& b2 = d2;
    ASSERT_EQ(crcOf("PolyBase"), typeHash(b2));
}

} // namespace

