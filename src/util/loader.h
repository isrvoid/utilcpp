#pragma once

#include <unordered_map>

#include <util/loaderifc.h>

namespace util {

// TODO RecordLoader which checks type info and version

class Record : IRecord {
public:
    Record(ConstPtr record);
    ConstPtr get(uint32_t id) const override;

    // TODO 2 versions specialized on is_trivial_serde trait
    template<typename T>
    T get(uint32_t id) const {
        auto entry = get(id);
        return *static_cast<const T*>(entry.ptr);
    }

private:
    std::unordered_map<uint32_t, ConstPtr> entries;
};

} // namespace util
