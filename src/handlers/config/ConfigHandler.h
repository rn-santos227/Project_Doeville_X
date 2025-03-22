#ifndef CONFIG_HANDLER_H
#define CONFIG_HANDLER_H

#include <string>
#include <unordered_map>

#include "utilities/logs_manager/LogsManager.h"

using namespace Project::Utilities;

namespace Project::Handlers {
  class ConfigHandler {
  public:
    explicit ConfigHandler(LogsManager& logsManager);

    bool loadConfig(const std::string& filePath);

    std::string getValue(const std::string& section, const std::string& key, const std::string& defaultValue = "") const;
    float getFloatValue(const std::string& section, const std::string& key, float defaultValue = 0.0f) const;
    int getIntValue(const std::string& section, const std::string& key, int defaultValue = 0) const;
    bool getBoolValue(const std::string& section, const std::string& key, bool defaultValue = false) const;

  private:
    LogsManager& logsManager;
    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> configData;
    
    void trim(std::string& str) const;
  };
}

#endif