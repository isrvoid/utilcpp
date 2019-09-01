#include <gtest/gtest.h>

#include <memory>

#include <util/typeinfo.h>
#include <util/digest/crc.h>

using namespace std;
using namespace util;
using namespace typeinfo;

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

TEST(TypeInfoTest, HashCode) {
    ASSERT_EQ(crcOf("int"), hashCode(42));
}

TEST(TypeInfoTest, HashCodeIsConstexpr) {
    constexpr auto typeHash = hashCode(42u);
    ASSERT_EQ(crcOf("unsigned int"), typeHash);
}

TEST(TypeInfoTest, HashCodeErasesTypedef) {
    ASSERT_EQ(crcOf("unsigned char"), hashCode(static_cast<uint8_t>(42)));
}

TEST(TypeInfoTest, HashCodeSharedPtr) {
    constexpr auto expect = crcOf("std::shared_ptr<int>");
    ASSERT_EQ(expect, hashCode(make_shared<int>(42)));
}

TEST(TypeInfoTest, HashCodeNestedTypes) {
    uint32_t expect;
    expect = crcOf("std::shared_ptr<std::vector<bool>>");
    ASSERT_EQ(expect, hashCode(make_shared<vector<bool>>()));

    expect = crcOf("std::vector<std::vector<std::pair<int, bool>>>");
    ASSERT_EQ(expect, hashCode(std::vector<std::vector<std::pair<int, bool>>>{}));
}

TEST(TypeInfoTest, HashCodeVoid) {
    ASSERT_EQ(crcOf("void"), TypeInfo<void>::hashCode());
}

TEST(TypeInfoTest, HashCodeIsNotPolymorphic) {
    NonPolyDerived d1;
    NonPolyBase& b1 = d1;
    ASSERT_EQ(crcOf("NonPolyBase"), hashCode(b1));

    PolyDerived d2;
    PolyBase& b2 = d2;
    ASSERT_EQ(crcOf("PolyBase"), hashCode(b2));
}

} // namespace

