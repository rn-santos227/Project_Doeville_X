#ifndef COMPRESSION_UTILS_H
#define COMPRESSION_UTILS_H

#include <vector>
#include <cstddef>

namespace Project::Utilities {
  struct CompressionUtils {
    static bool compress(const unsigned char* src, size_t srcSize, std::vector<unsigned char>& dest);
    static bool decompress(const unsigned char* src, size_t srcSize, std::vector<unsigned char>& dest);
    static bool decompressTo(const unsigned char* src, size_t srcSize, unsigned char* dest, size_t destSize, size_t& outSize);
  };
}

#endif
