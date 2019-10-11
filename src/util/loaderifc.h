#pragma once

#include <cstddef>
#include <cstdint>

namespace util {

struct ConstPtr {
    const void* ptr;
    size_t length;
};

class IRecord {
public:
    virtual ~IRecord() = default;
    virtual ConstPtr get(uint32_t id) const = 0;
};

class ILoader {
public:
    virtual ~ILoader() = default;
    virtual int load(const IRecord&) = 0;
};

// load() should return number of ids that couldn't be loaded.

} // namespace util
