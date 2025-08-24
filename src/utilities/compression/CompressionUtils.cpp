#include "CompressionUtils.h"
#include <zlib.h>

#include "libraries/constants/NumericConstants.h"
#include "libraries/constants/IndexConstants.h"

namespace Project::Utilities {
  namespace Constants = Project::Libraries::Constants;
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
    uLongf destLen = srcSize * Constants::INDEX_FOUR + Constants::ALLOC_1024;
    int res;
    do {
      dest.resize(destLen);
      res = ::uncompress(dest.data(), &destLen, src, srcSize);
      if (res == Z_BUF_ERROR) {
        destLen *= Constants::INDEX_TWO;
      }
    } while (res == Z_BUF_ERROR);
    if (res != Z_OK) {
      return false;
    }
    dest.resize(destLen);
    return true;
  }

  bool CompressionUtils::decompressTo(const unsigned char* src, size_t srcSize, unsigned char* dest, size_t destSize, size_t& outSize) {
    uLongf destLen = static_cast<uLongf>(destSize);
    int res = ::uncompress(dest, &destLen, src, srcSize);
    if (res != Z_OK) {
      return false;
    }
    outSize = static_cast<size_t>(destLen);
    return true;
  }
}
