#include "ConfigReader.h"

#include <fstream>
#include <sstream>
#include <algorithm>

namespace Project::Utilities {
  ConfigReader::ConfigReader(LogsManager& logsManager) : logsManager(logsManager) {}

  bool ConfigReader::loadConfig(const std::string& filePath) {
    std::ifstream file(filePath);
    if (logsManager.checkAndLogError(!file.is_open(), "Failed to open config file: " + filePath)) {
      logsManager.flushLogs();
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

   std::string ConfigReader::getValue(const std::string& section, const std::string& key, const std::string& defaultValue) const {
    auto secIt = configData.find(section);
    if (secIt != configData.end()) {

    }
   }

  void ConfigReader::trim(std::string& str) const {
    str.erase(0, str.find_first_not_of(" \t\r\n"));
    str.erase(str.find_last_not_of(" \t\r\n") + 1);
  }     
}