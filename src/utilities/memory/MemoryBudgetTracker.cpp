#include "MemoryBudgetTracker.h"

#include <limits>

namespace Project::Utilities {
  MemoryBudgetTracker::MemoryBudgetTracker(LogsManager& logs)
  : logsManager(logs) {
    budgets[MemorySystem::Textures] = std::numeric_limits<std::size_t>::max();
    budgets[MemorySystem::Meshes] = std::numeric_limits<std::size_t>::max();
    budgets[MemorySystem::Audio] = std::numeric_limits<std::size_t>::max();
    usage[MemorySystem::Textures] = 0;
    usage[MemorySystem::Meshes] = 0;
    usage[MemorySystem::Audio] = 0;
  }

  void MemoryBudgetTracker::setBudget(MemorySystem system, std::size_t bytes) {
    budgets[system] = bytes;
  }

  bool MemoryBudgetTracker::allocate(MemorySystem system, std::size_t bytes) {
    usage[system] += bytes;
    Profiler::getInstance().setMemoryUsage(systemName(system), usage[system]);
    if (usage[system] > budgets[system]) {
      logsManager.logWarning(std::string("Memory budget exceeded for ") + systemName(system));
      return false;
    }
    return true;
  }

  void MemoryBudgetTracker::release(MemorySystem system, std::size_t bytes) {
    if (usage[system] >= bytes) {
      usage[system] -= bytes;
    } else {
      usage[system] = 0;
    }
  }
}
