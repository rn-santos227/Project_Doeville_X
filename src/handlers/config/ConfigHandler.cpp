#include "ConfigHandler.h"
#include <fstream>
#include <sstream>
#include <algorithm>

namespace Project::Handlers {
  ConfigHandler& ConfigHandler::getInstance() {
    static ConfigHandler instance;
    return instance;
  }

  bool ConfigHandler::loadConfig(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
      return false;
    }

    std::string line, section;
    while (std::getline(file, line)) {

    }

    file.close();
    return true;
  }

  void ConfigHandler::trim(std::string& str) const {

  }
}