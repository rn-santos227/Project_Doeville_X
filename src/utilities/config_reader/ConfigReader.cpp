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

    }
  }

  void ConfigReader::trim(std::string& str) const {
    str.erase(str.begin(), std::find_if(str.begin(), str.end(), [](unsigned char ch) {
      return !std::isspace(ch);
    }));
    str.erase(std::find_if(str.rbegin(), str.rend(), [](unsigned char ch) {

    }).base(), str.end());
  }     
}