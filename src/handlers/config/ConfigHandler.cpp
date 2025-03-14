#include "ConfigHandler.h"
#include <fstream>
#include <sstream>
#include <algorithm>

namespace Project::Handlers {
  ConfigHandler::ConfigHandler(LogsManager& logsManager) : logsManager(logsManager) {}

  bool ConfigHandler::loadConfig(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
      logsManager.logError("Failed to open config file: " + filePath);
      return false;
    }

    std::string line, section;
    while (std::getline(file, line)) {
      trim(line);

      if (line.empty() || line[0] == ';' || line[0] == '#') continue;

      if (line[0] == '[' && line.back() == ']') {
        section = line.substr(1, line.size() - 2);
        trim(section);
      } else {
        size_t delimiterPos = line.find('=');
        if (delimiterPos != std::string::npos) {
          std::string key = line.substr(0, delimiterPos);
          std::string value = line.substr(delimiterPos + 1);

          trim(key);
          trim(value);
          configData[section][key] = value;
          logsManager.logMessage("Loaded config: [" + section + "] " + key + " = " + value);
        }
      }
    }

    file.close();
    logsManager.logMessage("Config file loaded successfully from " + filePath);
    return true;
  }

  std::string ConfigHandler::getValue(const std::string& section, const std::string& key, const std::string& defaultValue) const {
    auto secIt = configData.find(section);
    if (secIt != configData.end()) {
      auto keyIt = secIt->second.find(key);

      if (keyIt != secIt->second.end()) {
        logsManager.logMessage("Retrieved config: [" + section + "] " + key + " = " + keyIt->second);
        return keyIt->second;
      }
    }
    
    logsManager.logWarning("Config key not found: [" + section + "] " + key + ", using default: " + defaultValue);
    return defaultValue;
  }

  float ConfigHandler::getFloatValue(const std::string& section, const std::string& key, float defaultValue) const {
    std::string value = getValue(section, key);
    return !value.empty() ? std::stof(value) : defaultValue;
  }

  int ConfigHandler::getIntValue(const std::string& section, const std::string& key, int defaultValue) const {
    std::string value = getValue(section, key);
    return !value.empty() ? std::stoi(value) : defaultValue;
  }

  bool ConfigHandler::getBoolValue(const std::string& section, const std::string& key, bool defaultValue) const {
    std::string value = getValue(section, key);
    return !value.empty() ? (value == "true" || value == "1") : defaultValue;
  }

  void ConfigHandler::trim(std::string& str) const {
    str.erase(0, str.find_first_not_of(" \t\r\n"));
    str.erase(str.find_last_not_of(" \t\r\n") + 1);
  }
}