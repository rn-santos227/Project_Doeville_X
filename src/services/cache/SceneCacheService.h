#ifndef SCENE_CACHE_SERVICE_H
#define SCENE_CACHE_SERVICE_H

#include <cstdint>
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

#include "utilities/binary_cache/BinaryFileCache.h"
#include "utilities/logs_manager/LogsManager.h"

namespace Project::Services {
  class SceneCacheService {
  public:

  private:
    struct CacheStats {
      size_t hits = 0;
      size_t misses = 0;
      std::uint64_t cpuSavedNs = 0;
      std::uint64_t cacheMissesSaved = 0;
    };

    Project::Utilities::LogsManager& logsManager;
    Project::Utilities::BinaryFileCache lightProbeCache;
    Project::Utilities::BinaryFileCache pathfindingCache;
    Project::Utilities::BinaryFileCache shaderCache;

    std::unordered_map<std::string, CacheStats> stats;
    std::unordered_map<std::string, std::uint64_t> lastComputeTime;
    std::unordered_map<std::string, std::uint64_t> lastComputeMisses;
  };
}

#endif
