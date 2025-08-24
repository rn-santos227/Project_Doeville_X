#include "BinaryFileCache.h"

#include <filesystem>
#include <fstream>
#include <chrono>

#include "helpers/serialization/EndianHelper.h"
#include "libraries/constants/NumericConstants.h"

namespace Project::Utilities {
  namespace Constants = Project::Libraries::Constants;
  namespace fs = std::filesystem;

  BinaryFileCache::BinaryFileCache(const std::string& filePath) : cacheFilePath(filePath) {
    fs::create_directories(fs::path(cacheFilePath).parent_path());
    load();
  }

  static long long toSeconds(fs::file_time_type tp) {
    return std::chrono::duration_cast<std::chrono::seconds>(tp.time_since_epoch()).count();
  }

  void BinaryFileCache::load() {
    cache.clear();
    std::ifstream in(cacheFilePath, std::ios::binary);
    if (!in.is_open()) return;

    std::uint64_t entries = 0;
    Project::Helpers::readLittleEndian(in, entries);
    for (std::uint64_t i = 0; i < entries; ++i) {
      std::uint64_t pathSize = 0;
      Project::Helpers::readLittleEndian(in, pathSize);

      if (!in || pathSize > Constants::MAX_PATH_SIZE) {
        if (in && pathSize > 0) in.seekg(static_cast<std::streamoff>(pathSize), std::ios::cur);
        std::int64_t ts = 0;
        Project::Helpers::readLittleEndian(in, ts);
        std::uint64_t dataSize = 0;
        Project::Helpers::readLittleEndian(in, dataSize);
        if (in && dataSize > 0) in.seekg(static_cast<std::streamoff>(dataSize), std::ios::cur);
        if (!in) break;
        continue;
      }

      std::string path(pathSize, '\0');
      in.read(path.data(), static_cast<std::streamsize>(pathSize));

      std::int64_t ts = 0;
      Project::Helpers::readLittleEndian(in, ts);

      std::uint64_t dataSize = 0;
      Project::Helpers::readLittleEndian(in, dataSize);

      if (!in || dataSize > Constants::MAX_DATA_SIZE) {
        if (in && dataSize > 0) in.seekg(static_cast<std::streamoff>(dataSize), std::ios::cur);
        if (!in) break;
        continue;
      }

      std::vector<char> data(dataSize);
      if (dataSize > 0) in.read(data.data(), static_cast<std::streamsize>(dataSize));
      if (!in) break;
      if (ts == getTimestamp(path)) {
        cache[path] = {ts, std::move(data)};
      }
    }
  }

  void BinaryFileCache::save() const {
    std::ofstream out(cacheFilePath, std::ios::binary | std::ios::trunc);
    if (!out.is_open()) return;

    std::uint64_t entries = cache.size();
    Project::Helpers::writeLittleEndian(out, entries);
    for (const auto& [path, pair] : cache) {
      long long ts = getTimestamp(path);
      std::uint64_t pathSize = path.size();
      Project::Helpers::writeLittleEndian(out, pathSize);
      out.write(path.data(), static_cast<std::streamsize>(pathSize));
      Project::Helpers::writeLittleEndian(out, static_cast<std::int64_t>(ts));
      const auto& data = pair.second;
      std::uint64_t dataSize = data.size();
      Project::Helpers::writeLittleEndian(out, dataSize);
      if (dataSize > 0) out.write(data.data(), static_cast<std::streamsize>(dataSize));
    }
  }

  bool BinaryFileCache::getData(const std::string& path, std::vector<char>& outData) const {
    auto it = cache.find(path);
    if (it == cache.end()) return false;
    if (it->second.first != getTimestamp(path)) return false;
    outData = it->second.second;
    return true;
  }

  void BinaryFileCache::setData(const std::string& path, const std::vector<char>& data) {
    cache[path] = { getTimestamp(path), data };
  }

  void BinaryFileCache::removeData(const std::string& path) {
    cache.erase(path);
  }

  long long BinaryFileCache::getTimestamp(const std::string& path) {
    if (!fs::exists(path)) return 0;
    return toSeconds(fs::last_write_time(path));
  }
}
