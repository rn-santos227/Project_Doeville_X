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
    using LightProbeData = std::vector<float>;
    using PathfindingGrid = std::vector<int>;
    using ShaderBinary = std::vector<char>;

    explicit SceneCacheService(Project::Utilities::LogsManager& logsManager);
    ~SceneCacheService();

    bool getLightProbe(const std::string& sceneId, LightProbeData& outData, const std::function<LightProbeData()>& recompute);
    bool getPathfindingGrid(const std::string& sceneId, PathfindingGrid& outData, const std::function<PathfindingGrid()>& recompute);
    bool getCompiledShader(const std::string& sceneId, ShaderBinary& outData, const std::function<ShaderBinary()>& recompute);

    void invalidateScene(const std::string& sceneId);
    void logDiagnostics() const;

  private:
    struct CacheStats {
      size_t hits = 0;
      size_t misses = 0;
      std::uint64_t cpuSavedNs = 0;
      std::uint64_t cacheMissesSaved = 0;
    };

    static std::string makeKey(const std::string& sceneId, const std::string& suffix);
    static void serializeData(const LightProbeData& in, std::vector<char>& out);
    static void serializeData(const PathfindingGrid& in, std::vector<char>& out);
    static void serializeData(const ShaderBinary& in, std::vector<char>& out);
    static void deserializeData(const std::vector<char>& in, LightProbeData& out);
    static void deserializeData(const std::vector<char>& in, PathfindingGrid& out);
    static void deserializeData(const std::vector<char>& in, ShaderBinary& out);

    Project::Utilities::LogsManager& logsManager;
    Project::Utilities::BinaryFileCache lightProbeCache;
    Project::Utilities::BinaryFileCache pathfindingCache;
    Project::Utilities::BinaryFileCache shaderCache;

    std::unordered_map<std::string, CacheStats> stats;
    std::unordered_map<std::string, std::uint64_t> lastComputeTime;
    std::unordered_map<std::string, std::uint64_t> lastComputeMisses;

    template <typename T, typename Func>
    bool fetch(
      Project::Utilities::BinaryFileCache& cache, 
      const std::string& type,
      const std::string& sceneId,
      const std::string& suffix,
      T& outData,
      const Func& recompute
    );
  };
}

#endif
