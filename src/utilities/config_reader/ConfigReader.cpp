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
      }
    }
  }

  void ConfigReader::trim(std::string& str) const {
    str.erase(0, str.find_first_not_of(" \t\r\n"));
    str.erase(str.find_last_not_of(" \t\r\n") + 1);
  }     
}