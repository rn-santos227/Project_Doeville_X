#ifndef SCRIPT_CACHE_H
#define SCRIPT_CACHE_H

#include <string>
#include <unordered_map>
#include <vector>

namespace Project::Utilities {
  class ScriptFunctionCache {
  public:
    explicit ScriptFunctionCache(const std::string& filePath);

    void load();
    void save() const;

    bool getFunctions(const std::string& scriptPath, std::vector<std::string>& outFuncs) const;
    void setFunctions(const std::string& scriptPath, const std::vector<std::string>& funcs);

  private:
    static long long getTimestamp(const std::string& path);

    std::string cacheFilePath;
    std::unordered_map<std::string, std::pair<long long, std::vector<std::string>>> cache;
  };
}

#endif
