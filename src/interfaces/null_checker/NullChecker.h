#ifndef NULL_CHECKER_H
#define NULL_CHECKER_H

#include <string>
#include "utilities/logs_manager/LogsManager.h"

namespace Project::Utilities {
  template <typename T>
  inline bool checkNotNull(LogsManager& logsManager, T* ptr, const std::string& errorMsg) {
    if (!ptr) {
      logsManager.logError(errorMsg);
      return false;
    }
    return true;
  }
}

#endif
