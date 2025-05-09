#ifndef CONFIG_READER_H
#define CONFIG_READER_H

#include <string>
#include <unordered_map>

#include "utilities/logs_manager/LogsManager.h"

using namespace Project::Utilities;

namespace Project::Utilities {
  class ConfigReader {
  public:
    explicit ConfigReader(LogsManager& logsManager);

  };
}

#endif