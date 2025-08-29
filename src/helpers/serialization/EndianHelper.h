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

  template <typename T>
  inline T littleEndianToHost(T value) {
    return isLittleEndian() ? value : byteswap(value);
  }

  template <typename T>
  inline void writeLittleEndian(std::ostream& out, T value) {
    T le = hostToLittleEndian(value);
    out.write(reinterpret_cast<const char*>(&le), sizeof(T));
  }

  template <typename T>
  inline bool readLittleEndian(std::istream& in, T& value) {
    T tmp;
    in.read(reinterpret_cast<char*>(&tmp), sizeof(T));
    if (!in) return false;
    value = littleEndianToHost(tmp);
    return true;
  }

  template <typename T>
  inline void serializeVector(const std::vector<T>& in, std::vector<char>& out) {
    out.resize(in.size() * sizeof(T));
    for (size_t i = 0; i < in.size(); ++i) {
      T le = hostToLittleEndian(in[i]);
      std::memcpy(out.data() + i * sizeof(T), &le, sizeof(T));
    }
  }

  template <typename T>
  inline void deserializeVector(const std::vector<char>& in, std::vector<T>& out) {
    size_t count = in.size() / sizeof(T);
    out.resize(count);
    for (size_t i = 0; i < count; ++i) {
      T val;
      std::memcpy(&val, in.data() + i * sizeof(T), sizeof(T));
      out[i] = littleEndianToHost(val);
    }
  }
}

#endif
