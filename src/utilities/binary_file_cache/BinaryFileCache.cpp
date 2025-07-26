#include "BinaryFileCache.h"

#include <filesystem>
#include <fstream>
#include <chrono>

namespace Project::Utilities {
  namespace fs = std::filesystem;

  static long long toSeconds(fs::file_time_type tp) {
    return std::chrono::duration_cast<std::chrono::seconds>(tp.time_since_epoch()).count();
  }

  void BinaryFileCache::load() {
    cache.clear();
    std::ifstream in(cacheFilePath, std::ios::binary);
    if (!in.is_open()) return;

    size_t entries = 0;
    in.read(reinterpret_cast<char*>(&entries), sizeof(entries));
    for (size_t i = 0; i < entries; ++i) {

    }
  }

  long long BinaryFileCache::getTimestamp(const std::string& path) {
    if (!fs::exists(path)) return 0;
    return toSeconds(fs::last_write_time(path));
  }
}
