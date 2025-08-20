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

  std::string SceneCacheService::makeKey(const std::string& sceneId, const std::string& suffix) {
    return sceneId + "_" + suffix;
  }

  void SceneCacheService::serializeData(const LightProbeData& in, std::vector<char>& out) {
    out.resize(in.size() * sizeof(float));
    std::memcpy(out.data(), in.data(), out.size());
  }

  void SceneCacheService::serializeData(const PathfindingGrid& in, std::vector<char>& out) {
    out.resize(in.size() * sizeof(int));
    std::memcpy(out.data(), in.data(), out.size());
  }

  template <typename T, typename Func>
  bool SceneCacheService::fetch(
    BinaryFileCache& cache, 
    const std::string& type,
    const std::string& sceneId, 
    const std::string& suffix, 
    T& outData, 
    const Func& recompute) {

  }
}
