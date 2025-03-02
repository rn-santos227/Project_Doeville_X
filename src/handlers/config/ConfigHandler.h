#ifndef CONFIG_HANDLER_H
#define CONFIG_HANDLER_H

#include <string>
#include <unordered_map>

namespace Project::Handlers {
  class ConfigHandler {
  public:
    static ConfigHandler& getInstance();

    bool loadConfig(const std::string& filePath);
    bool getBoolValue(const std::string& section, const std::string& key, bool defaultValue = false) const;

  private:
    ConfigHandler() = default;

  };
}

#endif