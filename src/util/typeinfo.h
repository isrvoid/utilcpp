#pragma once

#include <type_traits>
#include <util/digest/crc.h>

namespace typeinfo {

template<typename T>
struct TypeInfo {
	static constexpr unsigned int hashCode() {
		util::digest::CRC32 crc;
		// __PRETTY_FUNCTION__:
		// static constexpr unsigned int typeinfo::TypeInfo<T>::hashCode() [with T = unsigned char]
		constexpr const char* func = __PRETTY_FUNCTION__;
		constexpr size_t squareBracketIndex = 64;
		static_assert('[' == func[squareBracketIndex], "Wrong '[' index (function signature changed?)");
		const char* p = func + squareBracketIndex + 9; // "9: with T = ".length();
		while (*++p != ']') {
			// skip spaces in closing '>' (e.g. std::shared_ptr<std::vector<int> >)
			if (*p == ' ' && p[-1] == '>')
				++p;

			crc.put(*p);
		}

		return crc.finish();
	}
};

template<typename T>
constexpr uint32_t hashCode(const T&) {
	static_assert(!std::is_pointer<T>::value, "Don't pass pointers (const ambiguity)");
	return TypeInfo<T>::hashCode();
}

} // namespace typeinfo

