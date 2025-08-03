#ifndef COMPRESSION_H
#define COMPRESSION_H

#include <vector>
#include <cstddef>

namespace Project::Utilities {
  struct Compression {
    static bool compress(const unsigned char* src, size_t srcSize, std::vector<unsigned char>& dest);
    static bool decompress(const unsigned char* src, size_t srcSize, std::vector<unsigned char>& dest);
  };
}

#endif
