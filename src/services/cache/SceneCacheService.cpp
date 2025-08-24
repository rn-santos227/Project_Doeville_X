#include "SceneCacheService.h"

#include <chrono>
#include <cstring>

#include "helpers/serialization/EndianHelper.h"
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

  bool SceneCacheService::getLightProbe(const std::string& sceneId, LightProbeData& outData, const std::function<LightProbeData()>& recompute) {
    return fetch(lightProbeCache, Constants::LIGHT_PROBE, sceneId, Constants::LIGHT_PROBE_SYM, outData, recompute);
  }

  bool SceneCacheService::getPathfindingGrid(const std::string& sceneId, PathfindingGrid& outData, const std::function<PathfindingGrid()>& recompute) {
    return fetch(pathfindingCache, Constants::PATHFINDING, sceneId, Constants::PATHFINDING_SYM, outData, recompute);
  }

  bool SceneCacheService::getCompiledShader(const std::string& sceneId, ShaderBinary& outData, const std::function<ShaderBinary()>& recompute) {
    return fetch(shaderCache, Constants::SHADER, sceneId, Constants::SHADER_SYM, outData, recompute);
  }

  void SceneCacheService::invalidateScene(const std::string& sceneId) {
    lightProbeCache.removeData(makeKey(sceneId, Constants::LIGHT_PROBE_SYM));
    pathfindingCache.removeData(makeKey(sceneId, Constants::PATHFINDING_SYM));
    shaderCache.removeData(makeKey(sceneId, Constants::SHADER_SYM));
  }

  void SceneCacheService::logDiagnostics() const {
    for (const auto& [type, stat] : stats) {
      logsManager.logMessage(
        type + " cache: hits=" + std::to_string(stat.hits) +
        ", misses=" + std::to_string(stat.misses) +
        ", cpuSavedNs=" + std::to_string(stat.cpuSavedNs) +
        ", cacheMissesSaved=" + std::to_string(stat.cacheMissesSaved)
      );   
    }
  }

  std::string SceneCacheService::makeKey(const std::string& sceneId, const std::string& suffix) {
    return sceneId + "_" + suffix;
  }

  void SceneCacheService::serializeData(const LightProbeData& in, std::vector<char>& out) {
    Project::Helpers::serializeVector(in, out);
  }

  void SceneCacheService::serializeData(const PathfindingGrid& in, std::vector<char>& out) {
    Project::Helpers::serializeVector(in, out);
  }

  void SceneCacheService::serializeData(const ShaderBinary& in, std::vector<char>& out) {
    out = in;
  }

  void SceneCacheService::deserializeData(const std::vector<char>& in, LightProbeData& out) {
    Project::Helpers::deserializeVector(in, out);
  }

  void SceneCacheService::deserializeData(const std::vector<char>& in, PathfindingGrid& out) {
    Project::Helpers::deserializeVector(in, out);
  }

  void SceneCacheService::deserializeData(const std::vector<char>& in, ShaderBinary& out) {
    out = in;
  }

  template <typename T, typename Func>
  bool SceneCacheService::fetch(
    BinaryFileCache& cache, 
    const std::string& type,
    const std::string& sceneId, 
    const std::string& suffix, 
    T& outData, 
    const Func& recompute) {
    const std::string key = makeKey(sceneId, suffix);
    std::vector<char> raw;
    CacheProfiler profiler;
    profiler.start();
    auto start = std::chrono::high_resolution_clock::now();
    bool hit = cache.getData(key, raw);
    auto end = std::chrono::high_resolution_clock::now();
    auto cachedMisses = profiler.stop();
    auto cachedNs = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

    auto& stat = stats[type];
    if (hit) {
      deserializeData(raw, outData);
      auto computeNs = lastComputeTime[key];
      auto computeMisses = lastComputeMisses[key];
      if (computeNs > cachedNs) stat.cpuSavedNs += computeNs - cachedNs;
      if (computeMisses > cachedMisses) stat.cacheMissesSaved += computeMisses - cachedMisses;
      stat.hits++;
      return true;
    }

    profiler.start();
    start = std::chrono::high_resolution_clock::now();
    outData = recompute();
    end = std::chrono::high_resolution_clock::now();
    auto missMisses = profiler.stop();
    auto missNs = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    lastComputeTime[key] = missNs;
    lastComputeMisses[key] = missMisses;
    serializeData(outData, raw);
    cache.setData(key, raw);
    stat.misses++;
    return false;
  }
}
