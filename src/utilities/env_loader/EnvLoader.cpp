#include "EnvLoader.h"

#include <cstdlib>
#include <fstream>

#include "libraries/constants/Constants.h"

namespace Project::Utilities {
  namespace Constants = Project::Libraries::Constants;
  EnvLoader::EnvLoader(LogsManager& logsManager, const std::string& filePath)
    : logsManager(logsManager), filePath(filePath) {}

  bool EnvLoader::loadEnv() {

  }

  void EnvLoader::trim(std::string& str) const {
    str.erase(0, str.find_first_not_of(Constants::REGEX_WHITESPACE));
    str.erase(str.find_last_not_of(Constants::REGEX_WHITESPACE) + Constants::INT_ONE);
  }
}