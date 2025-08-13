#include "MemoryBudgetTracker.h"

#include <limits>

#include "libraries/categories/MemorySystemCategories.h"

namespace Project::Utilities {
  namespace MemorySystems = Project::Libraries::Categories::MemorySystems;
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
    Profiler::getInstance().setMemoryUsage(systemName(system), usage[system]);
  }

  std::size_t MemoryBudgetTracker::getUsage(MemorySystem system) const {
    auto it = usage.find(system);
    if (it != usage.end()) return it->second;
    return 0;
  }

  const char* MemoryBudgetTracker::systemName(MemorySystem system) const {
    switch (system) {
      case MemorySystem::Textures: return MemorySystems::TEXTURE;
      case MemorySystem::Meshes: return MemorySystems::MESHES;
      case MemorySystem::Audio: return MemorySystems::AUDIO;
    }
    return MemorySystems::UNKNOWN;
  }
}
