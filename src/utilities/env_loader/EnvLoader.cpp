#include "EnvLoader.h"

#include <cstdlib>
#include <fstream>

#include "libraries/constants/RegexConstants.h"

namespace Project::Utilities {
  namespace Regex = Project::Libraries::Constants;
  EnvLoader::EnvLoader(LogsManager& logsManager, const std::string& filePath)
    : logsManager(logsManager), filePath(filePath) {}

  void EnvLoader::trim(std::string& str) const {
    str.erase(0, str.find_first_not_of(Regex::REGEX_WHITESPACE));
    str.erase(str.find_last_not_of(Regex::REGEX_WHITESPACE) + 1);
  }
}