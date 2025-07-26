#include "EntitySeeder.h"

#include <cmath>
#include <string>

#include <SDL.h>

#include "components/graphics_component/GraphicsComponent.h"
#include "entities/Entity.h"
#include "entities/EntitiesManager.h"
#include "factories/entity/EntitiesFactory.h"
#include "libraries/constants/Constants.h"
#include "libraries/keys/Keys.h"

namespace Project::Entities {
  using Project::Factories::EntitiesFactory;

  namespace Constants = Project::Libraries::Constants;
  namespace Keys = Project::Libraries::Keys;

  EntitySeeder::EntitySeeder(EntitiesManager& mgr, EntitiesFactory& fac)
  : manager(mgr), factory(fac) {
    std::random_device rd;
    baseSeed = rd();
    sessionSalt = rd();
    rng.seed(baseSeed ^ sessionSalt);
    entityTemplates.reserve(Project::Libraries::Constants::INT_TEN);
    distribution = [this](std::mt19937& r) { return countDistribution(r); };
  }

  void EntitySeeder::update(float deltaTime) {
    auto p = player.lock();
    if (!p) return;

    float px = p->getX();
    float py = p->getY();

    for (auto& [k, chunk] : chunks) {
      int cx = static_cast<int>(k >> 32);
      int cy = static_cast<int>(static_cast<unsigned int>(k));
      auto it = chunk.ids.begin();
      while (it != chunk.ids.end()) {
        auto ent = manager.getEntity(*it);
        if (!ent) {
          it = chunk.ids.erase(it);
          continue;
        }

        int ecx = static_cast<int>(std::floor(ent->getX() / chunkSize.x));
        int ecy = static_cast<int>(std::floor(ent->getY() / chunkSize.y));
        long long nk = key(ecx, ecy);

        if (nk != k) {
          chunks[nk].ids.push_back(*it);
          it = chunk.ids.erase(it);
        } else {
          ++it;
        }
      }
    }

    int pcx = static_cast<int>(std::floor(px / chunkSize.x));
    int pcy = static_cast<int>(std::floor(py / chunkSize.y));

    SDL_Rect cullRect{0,0,0,0};
    bool useCull = false;
    auto* camHandler = Project::Components::GraphicsComponent::getCameraHandler();
    if (camHandler) {
      cullRect = camHandler->getCullingRect();
      useCull = true;
    }

    for (int dx = -chunkRadius; dx <= chunkRadius; ++dx) {
      for (int dy = -chunkRadius; dy <= chunkRadius; ++dy) {
        int cx = pcx + dx;
        int cy = pcy + dy;
        long long k = key(cx, cy);
        if (!chunks.count(k) && scheduledChunks.insert(k).second) {
          if (useCull) {
            SDL_Rect chunkRect{ 
              static_cast<int>(cx * chunkSize.x), 
              static_cast<int>(cy * chunkSize.y),
              static_cast<int>(chunkSize.x), 
              static_cast<int>(chunkSize.y) 
            };
            if (!SDL_HasIntersection(&chunkRect, &cullRect)) {
              scheduledChunks.erase(k);
              continue;
            }
          }
          pendingChunks.push(k);
        }
      }
    }

    size_t loaded = 0;
    while (!pendingChunks.empty() && loaded < maxChunksPerFrame) {
      long long k = pendingChunks.front();
      pendingChunks.pop();
      scheduledChunks.erase(k);
      int cx = static_cast<int>(k >> 32);
      int cy = static_cast<int>(static_cast<unsigned int>(k));
      loadChunk(cx, cy);
      ++loaded;
    }

    std::vector<long long> remove;
    for (const auto& [k, _] : chunks) {
      int cx = static_cast<int>(k >> 32);
      int cy = static_cast<int>(static_cast<int>(k));
      if (std::abs(cx - pcx) > chunkRadius || std::abs(cy - pcy) > chunkRadius) {
        remove.push_back(k);
      }
    }
    for (long long k : remove) {
      unloadChunk(k);
    }
  }

  void EntitySeeder::setPlayer(std::shared_ptr<Entity> p) {
    player = p;
  }

  void EntitySeeder::setSeed(size_t seed) {
    baseSeed = seed;
    rng.seed(baseSeed ^ sessionSalt);
  }

  void EntitySeeder::setSeedString(const std::string& seedStr) {
    setSeed(std::hash<std::string>{}(seedStr));
  }

  void EntitySeeder::addEntityTemplate(const std::string& name) {
    entityTemplates.push_back(name);
  }

  void EntitySeeder::addEntityTemplate(const std::string& name, size_t count) {
    for (size_t i = 0; i < count; ++i) {
      entityTemplates.push_back(name);
    }
    size_t total = entityTemplates.size();
    countDistribution = std::uniform_int_distribution<size_t>(Constants::INDEX_ONE, total);
    distribution = [this](std::mt19937& r) { return countDistribution(r); };
  }

  size_t EntitySeeder::generateChunkSeed(size_t base, long long key) {
    base ^= static_cast<size_t>(key) +
            static_cast<size_t>(Constants::DEFAULT_SEEDER) +
            (base << Constants::INDEX_SIX) + (base >> Constants::INDEX_TWO);
    base ^= sessionSalt +
            (base << Constants::INDEX_THREE) + (base >> Constants::INDEX_FIVE);
    return std::hash<size_t>{}(base);
  }

  long long EntitySeeder::key(int x, int y) const {
    return (static_cast<long long>(x) << Constants::SEEDER_KEY) ^ static_cast<unsigned int>(y);
  }

  void EntitySeeder::loadChunk(int cx, int cy) {
    long long k = key(cx, cy);
    Chunk& chunk = chunks[k];
    if (!chunk.ids.empty()) return;

    size_t chunkSeed = generateChunkSeed(baseSeed, k);
    std::mt19937 localRng(chunkSeed);

    float speedFactor = Constants::DEFAULT_WHOLE;
    if (auto p = player.lock()) {
      auto* motion = dynamic_cast<Project::Components::MotionComponent*>(
          p->getComponent(Project::Libraries::Categories::Components::MOTION_COMPONENT));
      if (motion) {
        float maxSpeed = motion->getSpeed();
        float currentSpeed = motion->getCurrentSpeed();
        if (maxSpeed > 0.0f) {
          speedFactor += currentSpeed / maxSpeed;
        }
      }
    }

    size_t baseCount = distribution(localRng);
    size_t count = static_cast<size_t>(std::round(baseCount * speedFactor));
    if (count < 1) count = 1;
    std::uniform_real_distribution<float> posX(0.0f, chunkSize.x);
    std::uniform_real_distribution<float> posY(0.0f, chunkSize.y);
    std::uniform_int_distribution<size_t> templateIndex(0, entityTemplates.empty() ? 0 : entityTemplates.size() - 1);
    
    for (size_t i = 0; i < count; ++i) {
      if (entityTemplates.empty()) break;
      std::string tmpl = entityTemplates[templateIndex(localRng)];
      EntitiesFactory::EntityPtr entity = factory.cloneEntity(tmpl);
      if (!entity) continue;

      float ex = cx * chunkSize.x + posX(localRng);
      float ey = cy * chunkSize.y + posY(localRng);

      entity->getLuaStateWrapper().setGlobalNumber(Project::Libraries::Keys::X, ex);
      entity->getLuaStateWrapper().setGlobalNumber(Project::Libraries::Keys::Y, ey);
      entity->initialize();

      if (auto* box = entity->getBoundingBoxComponent()) {
        box->setEntityPosition(static_cast<int>(ex), static_cast<int>(ey));
      }

      std::shared_ptr<Entity> shared = std::move(entity);
      std::string id = tmpl + std::string(Constants::SEED) + std::to_string(idCounter++);
      std::string finalId = manager.addEntity(shared, id);
      chunk.ids.push_back(finalId);
    }
  }

  void EntitySeeder::unloadChunk(long long k) {
    auto it = chunks.find(k);
    if (it == chunks.end()) return;
    for (const std::string& id : it->second.ids) {
      manager.removeEntity(id);
    }
    chunks.erase(it);
  }
}
