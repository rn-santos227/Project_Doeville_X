#include "EnvLoader.h"

#include <cstdlib>
#include <fstream>

namespace Project::Utilities {
  EnvLoader::EnvLoader(LogsManager& logsManager, const std::string& filePath)
      : logsManager(logsManager), filePath(filePath) {}

}