#ifndef ENV_LOADER_H
#define ENV_LOADER_H

#include <string>
#include <unordered_map>

#include "utilities/logs_manager/LogsManager.h"

namespace Project::Utilities {
  class EnvLoader {
  public:
    EnvLoader(Project::Utilities::LogsManager& logsManager, const std::string& filePath);

    bool loadEnv();

    std::string getValue(const std::string& key, const std::string& defaultValue = "") const;
    const std::unordered_map<std::string, std::string>& getAll() const;

  private:
    Project::Utilities::LogsManager& logsManager;
    
    std::string filePath;
    std::unordered_map<std::string, std::string> values;

    void trim(std::string& str) const;
  };
}

#endif
