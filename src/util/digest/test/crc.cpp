#include <gtest/gtest.h>

#include <util/digest/crc.h>

using namespace std;
using namespace util::digest;

namespace {

constexpr uint32_t crc32Of(const char* s, uint32_t prevCrc = 0) {
	CRC32 crc(prevCrc);
	while (*s != '\0')
		crc.put(static_cast<uint8_t>(*s++));

	return crc.finish();
}

TEST(Crc, EmptyInput) {
	constexpr uint32_t res = crc32Of("");
	ASSERT_EQ(res, 0);
}

TEST(Crc, Abc) {
	constexpr uint32_t res = crc32Of("abc");
	ASSERT_EQ(0x352441c2, res);
}

TEST(Crc, ZeroTruncatesString) {
	// not nice (not true length), but required for compatibility
	ASSERT_EQ(crc32Of(""), crc32Of("\0"));
	ASSERT_EQ(crc32Of(""), crc32Of("\0foo"));
	ASSERT_EQ(crc32Of("abc"), crc32Of("abc\0foo"));
}

TEST(Crc, OneToNine) {
	constexpr uint32_t res = crc32Of("123456789");
	ASSERT_EQ(0xcbf43926, res);
}

TEST(Crc, Long) {
	constexpr const char* s = "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";
	constexpr uint32_t res = crc32Of(s);
	ASSERT_EQ(0x171a3f5f, res);
}

TEST(Crc, Longer) {
	constexpr const char* s = "abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu" ;
	constexpr uint32_t res = crc32Of(s);
	ASSERT_EQ(0x191f3349, res);
}

constexpr uint32_t getA1KCrc() {
	CRC32 crc;
	for (size_t i = 0; i < 1000; ++i)
		crc.put('a');

	return crc.finish();
}

TEST(Crc, A1K) {
	constexpr uint32_t res = getA1KCrc();
	ASSERT_EQ(0x9a38da03, res);
}

TEST(Crc, Continue) {
	constexpr uint32_t intermediate = crc32Of("ab");
	constexpr uint32_t res = crc32Of("c", intermediate);
	ASSERT_EQ(crc32Of("abc"), res);
}

TEST(Crc, FinishResetsState) {
	CRC32 crc;
	crc.put('x');
	crc.finish();
	crc.put('a');
	crc.put('b');
	crc.put('c');
	ASSERT_EQ(crc32Of("abc"), crc.finish());
}

TEST(Crc, Peek) {
	CRC32 crc;
	ASSERT_EQ(crc32Of(""), crc.peek());
	crc.put('a');
	crc.put('b');
	ASSERT_EQ(crc32Of("ab"), crc.peek());
}

TEST(Crc, PeekDoesNotAffectState) {
	CRC32 crc;
	crc.peek();
	crc.put('a');
	crc.peek();
	crc.put('b');
	crc.put('c');
	ASSERT_EQ(crc32Of("abc"), crc.finish());
}

} // namespace
