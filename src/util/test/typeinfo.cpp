#include <gtest/gtest.h>

#include <memory>

#include <util/typeinfo.h>
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

} // namespace
