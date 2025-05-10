#include "ConfigReader.h"

#include <fstream>
#include <sstream>
#include <algorithm>

namespace Project::Utilities {
  ConfigReader::ConfigReader(LogsManager& logsManager) : logsManager(logsManager) {}

  bool ConfigReader::loadConfig(const std::string& filePath) {
    std::ifstream file(filePath);
  }
}