#ifndef BINARY_FILE_CACHE_H
#define BINARY_FILE_CACHE_H

#include <string>
#include <unordered_map>
#include <vector>

namespace Project::Utilities {
  class BinaryFileCache {
  public:
    explicit BinaryFileCache(const std::string& filePath);

    void load();
    void save() const;

    bool getData(const std::string& path, std::vector<char>& outData) const;
    void setData(const std::string& path, const std::vector<char>& data);
    void removeData(const std::string& path);

  private:
    std::unordered_map<std::string, std::pair<long long, std::vector<char>>> cache;
    std::string cacheFilePath;
    
    static long long getTimestamp(const std::string& path);
  };
}

#endif
