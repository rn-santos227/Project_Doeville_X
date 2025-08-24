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

  template <typename T>
  inline T byteswap(T value) {
    static_assert(std::is_arithmetic<T>::value, "T must be arithmetic");
    std::array<std::uint8_t, sizeof(T)> bytes{};
    std::memcpy(bytes.data(), &value, sizeof(T));
    std::reverse(bytes.begin(), bytes.end());
    std::memcpy(&value, bytes.data(), sizeof(T));
    return value;
  }
  
  template <typename T>
  inline T hostToLittleEndian(T value) {
    return isLittleEndian() ? value : byteswap(value);
  }
}

#endif
