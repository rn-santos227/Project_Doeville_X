#ifndef ENDIAN_HELPER_H
#define ENDIAN_HELPER_H

#include <algorithm>
#include <array>
#include <cstdint>
#include <cstring>
#include <istream>
#include <type_traits>
#include <ostream>
#include <vector>

namespace Project::Helpers {
  inline bool isLittleEndian() {
    const std::uint16_t one = 1;
    return *reinterpret_cast<const std::uint8_t*>(&one) == 1;
  }
}

#endif
