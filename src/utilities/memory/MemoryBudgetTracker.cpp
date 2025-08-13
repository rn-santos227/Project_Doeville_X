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
}
