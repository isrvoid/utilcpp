#include "loader.h"

namespace util {

Record::Record(ConstPtr) {
    // FIXME
}

ConstPtr Record::get(uint32_t id) const {
    auto search = entries.find(id);
    return search != entries.end() ? search->second : ConstPtr{};
}

} // namespace util
