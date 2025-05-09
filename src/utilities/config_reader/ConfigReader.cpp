#ifndef CONFIG_READER_H
#define CONFIG_READER_H

#include <string>
#include <unordered_map>

#include "utilities/logs_manager/LogsManager.h"

using namespace Project::Utilities;

namespace Project::Utilities {
  class ConfigReader {
  public:
    explicit ConfigReader(LogsManager& logsManager);

    bool loadConfig(const std::string& filePath);

    std::string getValue(const std::string& section, const std::string& key, const std::string& defaultValue = "") const;
    float getFloatValue(const std::string& section, const std::string& key, float defaultValue = 0.0f) const;

  };
}

#endif