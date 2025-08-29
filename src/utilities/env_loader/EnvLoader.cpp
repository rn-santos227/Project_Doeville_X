#include "EnvLoader.h"

#include <cstdlib>
#include <fstream>

#include "libraries/constants/Constants.h"

namespace Project::Utilities {
  namespace Constants = Project::Libraries::Constants;
  EnvLoader::EnvLoader(LogsManager& logsManager, const std::string& filePath)
    : logsManager(logsManager), filePath(filePath) {}

  bool EnvLoader::loadEnv() {
    values.clear();
    std::ifstream file(filePath);
    if (!file.is_open()) {
      return false;
    }

    std::string line;
    while (std::getline(file, line)) {
      trim(line);
      if (line.empty() || line[0] == '#') continue;
      size_t pos = line.find('=');
      if (pos == std::string::npos) continue;
      std::string key = line.substr(0, pos);
      std::string value = line.substr(pos + 1);
      trim(key);
      trim(value);
      values[key] = value;
    }

    file.close();
    logsManager.logMessage("Environment file loaded from " + filePath);
    return true;
  }

  std::string EnvLoader::getValue(const std::string& key, const std::string& defaultValue) const {
    auto it = values.find(key);
    if (it != values.end()) {
      return it->second;
    }

    const char* envVal = std::getenv(key.c_str());
    if (envVal) {
      return std::string(envVal);
    }

    logsManager.logWarning("Env key not found: " + key + ", using default: " + defaultValue);
    return defaultValue;
  }


  const std::unordered_map<std::string, std::string>& EnvLoader::getAll() const {
    return values;
  }

  void EnvLoader::trim(std::string& str) const {
    str.erase(0, str.find_first_not_of(Constants::REGEX_WHITESPACE));
    str.erase(str.find_last_not_of(Constants::REGEX_WHITESPACE) + Constants::INT_ONE);
  }
}