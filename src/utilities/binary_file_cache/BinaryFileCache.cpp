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
     size_t pathSize = 0;
      in.read(reinterpret_cast<char*>(&pathSize), sizeof(pathSize));
      std::string path(pathSize, '\0');
      in.read(path.data(), pathSize);

      long long ts = 0;
      in.read(reinterpret_cast<char*>(&ts), sizeof(ts));

      size_t dataSize = 0;
      in.read(reinterpret_cast<char*>(&dataSize), sizeof(dataSize));
      std::vector<char> data(dataSize);
      if (dataSize > 0) in.read(data.data(), dataSize);

      if (ts == getTimestamp(path)) {
        cache[path] = {ts, std::move(data)};
      }
    }
  }

  void BinaryFileCache::save() const {
    std::ofstream out(cacheFilePath, std::ios::binary | std::ios::trunc);
    if (!out.is_open()) return;

    size_t entries = cache.size();
    out.write(reinterpret_cast<const char*>(&entries), sizeof(entries));
    for (const auto& [path, pair] : cache) {
      long long ts = getTimestamp(path);
      size_t pathSize = path.size();
      out.write(reinterpret_cast<const char*>(&pathSize), sizeof(pathSize));
      out.write(path.data(), pathSize);
      out.write(reinterpret_cast<const char*>(&ts), sizeof(ts));
      const auto& data = pair.second;
      size_t dataSize = data.size();
      out.write(reinterpret_cast<const char*>(&dataSize), sizeof(dataSize));
      if (dataSize > 0) out.write(data.data(), dataSize);
    }
  }

  long long BinaryFileCache::getTimestamp(const std::string& path) {
    if (!fs::exists(path)) return 0;
    return toSeconds(fs::last_write_time(path));
  }
}
