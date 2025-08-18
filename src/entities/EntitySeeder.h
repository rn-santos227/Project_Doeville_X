#ifndef ENTITY_SEEDER_H
#define ENTITY_SEEDER_H

#include "ChunkSize.h"

#include <atomic>
#include <cstddef>
#include <functional>
#include <memory>
#include <queue>
#include <random>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <SDL.h>

#include "interfaces/update_interface/Updatable.h"
#include "libraries/constants/IndexConstants.h"
#include "libraries/constants/FloatConstants.h"
#include "libraries/constants/NumericConstants.h"

namespace Project::Factories { class EntitiesFactory; }
namespace Project::Entities {
  class Entity;
  class EntitiesManager;

  class EntitySeeder : public Project::Interfaces::Updatable {
  public:
    using Distribution = std::function<size_t(std::mt19937&)>;
    EntitySeeder(EntitiesManager& manager, Project::Factories::EntitiesFactory& factory);

    size_t getTemplateCount() const { return entityTemplates.size(); }
    void update(float deltaTime) override;

    void setDistribution(Distribution distFunc) { distribution = std::move(distFunc); }
    void setPlayer(std::shared_ptr<Entity> player);

    void setSeed(size_t seed);
    void setSeedString(const std::string& seedStr);

    void addEntityTemplate(const std::string& name);
    void addEntityTemplate(const std::string& name, size_t count);

    void setSpawnRadius(float radius) { spawnRadius = radius; }
    void setChunkSize(const ChunkSize& _size) { chunkSize = _size; }
    void setChunkRadius(int radius) { chunkRadius = radius; }
    void setMaxSeededEntities(size_t max) { maxSeededEntities = max; }
    void setMaxEntitiesPerChunk(size_t max) { maxEntitiesPerChunk = max; }

    void renderDebug(SDL_Renderer* renderer);

  private:
    struct Chunk {
      std::vector<std::string> ids;
    };

    ChunkSize chunkSize{
      Project::Libraries::Constants::DEFAULT_CHUNK_SIZE,
      Project::Libraries::Constants::DEFAULT_CHUNK_SIZE,
      0.0f
    };

    Distribution distribution;
    std::uniform_int_distribution<size_t> countDistribution{Project::Libraries::Constants::INDEX_ONE, Project::Libraries::Constants::INDEX_THREE};
    
    std::weak_ptr<Entity> player;
    EntitiesManager& manager;
    Project::Factories::EntitiesFactory& factory;

    std::queue<long long> pendingChunks;
    std::queue<std::function<void()>> spawnQueue;
    std::unordered_map<long long, Chunk> chunks;
    std::unordered_set<long long> scheduledChunks;
    std::vector<std::string> entityTemplates;
    std::vector<std::string> spawnedIds;

    std::mt19937 rng;
    std::uniform_real_distribution<float> dist{0.0f, Project::Libraries::Constants::DEFAULT_WHOLE};

    float spawnRadius = Project::Libraries::Constants::DEFAULT_SPAWN_RADIUS;
    int chunkRadius = Project::Libraries::Constants::INT_ONE;

    std::atomic<size_t> idCounter{0};
    size_t baseSeed = 0;
    size_t sessionSalt = 0;
    size_t maxChunksPerFrame = 1;
    size_t maxSeededEntities = Project::Libraries::Constants::MAX_SEEDED_ENTITY;
    size_t maxEntitiesPerChunk = Project::Libraries::Constants::DEFAULT_ENTITIES_PER_CHUNK;
    size_t maxSpawnPerFrame = Project::Libraries::Constants::INDEX_FIVE;

    int lastCameraX = 0;
    int lastCameraY = 0;
    bool hasCameraPos = false;

    size_t generateChunkSeed(size_t base, long long key);

    long long key(int x, int y) const;
    void loadChunk(int cx, int cy);
    void unloadChunk(long long k);
  };
}

#endif
