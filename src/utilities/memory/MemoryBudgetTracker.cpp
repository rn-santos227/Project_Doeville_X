#include "MemoryBudgetTracker.h"

#include <limits>

namespace Project::Utilities {
  MemoryBudgetTracker::MemoryBudgetTracker(LogsManager& logs)
  : logsManager(logs) {

  }
}
