#include "EntitySeeder.h"

#include <cmath>
#include <string>

#include "entities/Entity.h"
#include "entities/EntitiesManager.h"
#include "factories/entity/EntitiesFactory.h"
#include "libraries/constants/Constants.h"
#include "libraries/keys/Keys.h"

namespace Project::Entities {
  namespace Constants = Project::Libraries::Constants;
  namespace Keys = Project::Libraries::Keys;

  EntitySeeder::EntitySeeder(EntitiesManager& mgr, Project::Factories::EntitiesFactory& fac)
  : manager(mgr), factory(fac) {
    std::random_device rd;
    baseSeed = rd();
    rng.seed(baseSeed);
  }

  void EntitySeeder::setPlayer(std::shared_ptr<Entity> p) {
    player = p;
  }

  void EntitySeeder::setSeed(size_t seed) {
    baseSeed = seed;
    rng.seed(baseSeed);
  }

  void EntitySeeder::setSeedString(const std::string& seedStr) {
    setSeed(std::hash<std::string>{}(seedStr));
  }

  void EntitySeeder::addEntityTemplate(const std::string& name) {
    entityTemplates.push_back(name);
  }

  void EntitySeeder::addEntityTemplate(const std::string& name, size_t count) {

  }

  long long EntitySeeder::key(int x, int y) const {
    return (static_cast<long long>(x) << 32) ^ static_cast<unsigned int>(y);
  }

  void EntitySeeder::loadChunk(int cx, int cy) {
    long long k = key(cx, cy);
    Chunk& chunk = chunks[k];
    if (!chunk.ids.empty()) return;

    size_t chunkSeed = std::hash<long long>{}(static_cast<long long>(baseSeed) ^ k);
    std::mt19937 localRng(chunkSeed);

    size_t count = distribution(localRng);
    std::uniform_real_distribution<float> pos(0.0f, chunkSize);
      for (size_t i = 0; i < count; ++i) {
      if (entityTemplates.empty()) break;
      std::string tmpl = entityTemplates[idCounter % entityTemplates.size()];
      std::unique_ptr<Entity> entity = factory.cloneEntity(tmpl);
      if (!entity) continue;

      float ex = cx * chunkSize + pos(localRng);
      float ey = cy * chunkSize + pos(localRng);

      entity->getLuaStateWrapper().setGlobalNumber(Project::Libraries::Keys::X, ex);
      entity->getLuaStateWrapper().setGlobalNumber(Project::Libraries::Keys::Y, ey);
      entity->initialize();

      std::shared_ptr<Entity> shared = std::move(entity);
      std::string id = tmpl + std::string(Constants::SEED) + std::to_string(idCounter++);
      manager.addEntity(id, shared);
      chunk.ids.push_back(id);
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

        int ecx = static_cast<int>(std::floor(ent->getX() / chunkSize));
        int ecy = static_cast<int>(std::floor(ent->getY() / chunkSize));
        long long nk = key(ecx, ecy);

        if (nk != k) {
          chunks[nk].ids.push_back(*it);
          it = chunk.ids.erase(it);
        } else {
          ++it;
        }
      }
    }

    int pcx = static_cast<int>(std::floor(px / chunkSize));
    int pcy = static_cast<int>(std::floor(py / chunkSize));

    for (int dx = -chunkRadius; dx <= chunkRadius; ++dx) {
      for (int dy = -chunkRadius; dy <= chunkRadius; ++dy) {
        int cx = pcx + dx;
        int cy = pcy + dy;
        long long k = key(cx, cy);
        if (!chunks.count(k)) {
          loadChunk(cx, cy);
        }
      }
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
}
