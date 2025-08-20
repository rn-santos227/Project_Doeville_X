#include "SceneCacheService.h"

#include <chrono>
#include <cstring>

#include "libraries/constants/NameConstants.h"
#include "libraries/constants/PathConstants.h"
#include "utilities/profiler/CacheProfiler.h"

namespace Project::Services {
  using Project::Utilities::LogsManager;
  using Project::Utilities::BinaryFileCache;
  using Project::Utilities::CacheProfiler;

  namespace Constants = Project::Libraries::Constants;
  SceneCacheService::SceneCacheService(LogsManager& logs)
  : logsManager(logs),
    lightProbeCache(Constants::LIGHT_PROBE_CACHE_FILE),
    pathfindingCache(Constants::PATHFINDING_CACHE_FILE),
    shaderCache(Constants::SHADER_CACHE_FILE) {}

  SceneCacheService::~SceneCacheService() {
    lightProbeCache.save();
    pathfindingCache.save();
    shaderCache.save();
  }
}
