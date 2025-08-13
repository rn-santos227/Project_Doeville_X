#ifndef MEMORY_BUDGET_TRACKER_H
#define MEMORY_BUDGET_TRACKER_H

#include "MemorySystem.h"

#include <cstddef>
#include <unordered_map>

#include "utilities/logs_manager/LogsManager.h"
#include "utilities/profiler/Profiler.h"

namespace Project::Utilities {
  class MemoryBudgetTracker {
  public:
    explicit MemoryBudgetTracker(LogsManager& logs);

  private:
    LogsManager& logsManager;
    std::unordered_map<MemorySystem, std::size_t> budgets;
    std::unordered_map<MemorySystem, std::size_t> usage;

    const char* systemName(MemorySystem system) const;

  };
}

#endif
