#pragma once

#include <cassert>
#include <type_traits>

#include <util/digest/crc.h>
#include <util/typeinfo.h>

#define ASPECT_ID_BASE(_ENUM, _ASPECT) ::util::_AspectIdBase<_ENUM>::get( \
    _ENUM##Name[static_cast<uint32_t>(_ENUM::_ASPECT)])

namespace util {

template<typename Aspect, typename = std::enable_if_t<std::is_enum<Aspect>::value>>
struct _AspectIdBase {
    static constexpr uint32_t get(const char* name) {
        digest::CRC32 crc{TypeHash<Aspect>::hash};
        crc.put(':');
        crc.put(':');
        while (*name != '\0')
            crc.put(*name++);

        return crc.finish();
    }
};

constexpr uint32_t aspectIdMaybeAddIndex(uint32_t id, bool shouldAdd, size_t index) {
    assert(index < 1 << 8);
    if (!shouldAdd)
        return id;

    digest::CRC32 crc{id};
    crc.put(static_cast<uint8_t>(index));
    return crc.finish();
}

} // namespace util
