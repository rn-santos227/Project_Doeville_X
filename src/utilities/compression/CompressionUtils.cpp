#include "CompressionUtils.h"
#include <zlib.h>

namespace Project::Utilities {
  bool CompressionUtils::compress(const unsigned char* src, size_t srcSize, std::vector<unsigned char>& dest) {
    uLongf destLen = compressBound(srcSize);
    dest.resize(destLen);
    int res = ::compress(dest.data(), &destLen, src, srcSize);
    if (res != Z_OK) {
      return false;
    }
    dest.resize(destLen);
    return true;
  }

  bool CompressionUtils::decompress(const unsigned char* src, size_t srcSize, std::vector<unsigned char>& dest) {
    uLongf destLen = srcSize * 4 + 1024;
    int res;
    do {
      dest.resize(destLen);
      res = ::uncompress(dest.data(), &destLen, src, srcSize);
      if (res == Z_BUF_ERROR) {
        destLen *= 2;
      }
    } while (res == Z_BUF_ERROR);
    if (res != Z_OK) {
      return false;
    }
    dest.resize(destLen);
    return true;
  }
}
