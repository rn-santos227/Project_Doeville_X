#include "EntitySeeder.h"

#include <algorithm>
#include <cmath>
#include <string>

#include <SDL.h>

#include "components/graphics_component/GraphicsComponent.h"
#include "components/motion_component/MotionComponent.h"
#include "entities/Entity.h"
#include "entities/EntitiesManager.h"
#include "factories/entity/EntitiesFactory.h"
#include "libraries/constants/Constants.h"
#include "libraries/keys/Keys.h"
#include "states/DimensionMode.h"
#include "states/GameState.h"

namespace Project::Entities {
  using Project::Factories::EntitiesFactory;
  using Project::States::DimensionMode;

  namespace Constants = Project::Libraries::Constants;
  namespace Keys = Project::Libraries::Keys;

  EntitySeeder::EntitySeeder(EntitiesManager& mgr, EntitiesFactory& fac)
  : manager(mgr), factory(fac) {
    std::random_device rd;
    baseSeed = rd();
    sessionSalt = rd();
    rng.seed(baseSeed ^ sessionSalt);
    entityTemplates.reserve(Constants::INT_TEN);
    distribution = [this](std::mt19937& r) { return countDistribution(r); };
  }

  void EntitySeeder::update(float deltaTime) {
    auto p = player.lock();
    if (!p) return;

    float px = p->getX();
    float py = p->getY();

    for (auto& [k, chunk] : chunks) {
      int cx = static_cast<int>(k >> Constants::BIT_32);
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

    SDL_FRect cullRect{0,0,0,0};
    SDL_FRect expandedCull{0,0,0,0};
    bool useCull = false;
    bool bounded = false;
    SDL_Rect mapRect{0,0,0,0};

    auto* camHandler = Project::Components::GraphicsComponent::getCameraHandler();
    
    if (camHandler) {
      SDL_FRect fr = camHandler->getCullingRect();
      cullRect = {fr.x, fr.y, fr.w, fr.h};

      int camX = camHandler->getX();
      int camY = camHandler->getY();
      if (!hasCameraPos) {
        lastCameraX = camX;
        lastCameraY = camY;
        hasCameraPos = true;
      } else {
        int dx = camX - lastCameraX;
        int dy = camY - lastCameraY;
        int extraX = std::min(std::abs(dx), camHandler->getOffsetX());
        int extraY = std::min(std::abs(dy), camHandler->getOffsetY());
        if (dx < 0) {
          cullRect.x -= extraX;
          cullRect.w += extraX;
        } else if (dx > 0) {
          cullRect.w += extraX;
        }
        if (dy < 0) {
          cullRect.y -= extraY;
          cullRect.h += extraY;
        } else if (dy > 0) {
          cullRect.h += extraY;
        }
        lastCameraX = camX;
        lastCameraY = camY;
      }

      expandedCull = cullRect;
      expandedCull.x -= static_cast<int>(chunkSize.x);
      expandedCull.y -= static_cast<int>(chunkSize.y);
      expandedCull.w += static_cast<int>(Constants::INDEX_TWO * chunkSize.x);
      expandedCull.h += static_cast<int>(Constants::INDEX_TWO * chunkSize.y);

      useCull = true;
    }
    
    if (auto *state = manager.getGameState()) {
      auto mode = state->getDimensionMode();
      if (mode == DimensionMode::BOUNDED || mode == DimensionMode::MAPPED) {
        mapRect = state->getMapRect();
        bounded = mapRect.w > 0 && mapRect.h > 0;
      }
    }

    for (int dx = -chunkRadius; dx <= chunkRadius; ++dx) {
      for (int dy = -chunkRadius; dy <= chunkRadius; ++dy) {
        int cx = pcx + dx;
        int cy = pcy + dy;
        long long k = key(cx, cy);
        if (!chunks.count(k) && scheduledChunks.insert(k).second) {
          if (useCull) {
            SDL_FRect chunkRect{
              cx * chunkSize.x,
              cy * chunkSize.y,
              chunkSize.x, chunkSize.y
            };
            if (!SDL_HasIntersectionF(&chunkRect, &cullRect)) {
              scheduledChunks.erase(k);
              continue;
            }
          }
          if (bounded) {
            SDL_Rect chunkRect{
              static_cast<int>(cx * chunkSize.x),
              static_cast<int>(cy * chunkSize.y),
              static_cast<int>(chunkSize.x),
              static_cast<int>(chunkSize.y)
            };
            if (!SDL_HasIntersection(&chunkRect, &mapRect)) {
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
      int cx = static_cast<int>(k >> Constants::BIT_32);
      int cy = static_cast<int>(static_cast<unsigned int>(k));
      loadChunk(cx, cy);
      ++loaded;
    }

    std::vector<long long> remove;
    for (const auto& [k, chunk] : chunks) {
      int cx = static_cast<int>(k >> Constants::BIT_32);
      int cy = static_cast<int>(static_cast<unsigned int>(k));

      bool outsideRadius = std::abs(cx - pcx) > chunkRadius || std::abs(cy - pcy) > chunkRadius;

      if (outsideRadius && useCull) {
        SDL_FRect chunkRect{
          cx * chunkSize.x,
          cy * chunkSize.y,
          chunkSize.x, chunkSize.y
        };
        if (SDL_HasIntersectionF(&chunkRect, &expandedCull)) {
          continue;
        }

        bool entityVisible = false;
        for (const std::string& id : chunk.ids) {
          auto ent = manager.getEntity(id);
          if (!ent) continue;
          SDL_FRect r{ent->getX(), ent->getY(), 1.0f, 1.0f};
          if (auto* gfx = ent->getGraphicsComponent()) {
            int w = gfx->getWidth();
            int h = gfx->getHeight();
            r.w = w > 0 ? w : 1;
            r.h = h > 0 ? h : 1;
          } else if (auto* box = ent->getBoundingBoxComponent()) {
            const auto& boxes = box->getBoxes();
            if (!boxes.empty()) {
              r = boxes.front();
              if (r.w <= 0) r.w = 1;
              if (r.h <= 0) r.h = 1;
            }
          }
          if (SDL_HasIntersectionF(&r, &expandedCull)) {
            entityVisible = true;
            break;
          }
        }
        if (entityVisible) {
          continue;
        }
      }

      if (outsideRadius) {
        remove.push_back(k);
      }
    }
    for (long long k : remove) {
      unloadChunk(k);
    }

    size_t processed = 0;
    while (!spawnQueue.empty() && processed < maxSpawnPerFrame) {
      auto task = std::move(spawnQueue.front());
      spawnQueue.pop();
      task();
      ++processed;
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

  void EntitySeeder::renderDebug(SDL_Renderer* renderer) {
    if (!renderer) return;
    auto* camHandler = Project::Components::GraphicsComponent::getCameraHandler();
    const int camX = camHandler ? camHandler->getX() : 0;
    const int camY = camHandler ? camHandler->getY() : 0;

    SDL_SetRenderDrawColor(renderer, 0, Constants::BIT_255, 0, Constants::BIT_255);
    for (const auto& [k, _] : chunks) {
      int cx = static_cast<int>(k >> Constants::BIT_32);
      int cy = static_cast<int>(static_cast<unsigned int>(k));
      SDL_Rect rect{
        static_cast<int>(cx * chunkSize.x) - camX,
        static_cast<int>(cy * chunkSize.y) - camY,
        static_cast<int>(chunkSize.x),
        static_cast<int>(chunkSize.y)
      };
      SDL_RenderDrawRect(renderer, &rect);
    }
  }  

  size_t EntitySeeder::generateChunkSeed(size_t base, long long key) {
    base ^= static_cast<size_t>(key) +
            static_cast<size_t>(Constants::DEFAULT_HASH) +
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
    if (manager.getEntityCount() >= maxSeededEntities) return;

    bool bounded = false;
    SDL_Rect mapRect{0,0,0,0};
    if (auto* state = manager.getGameState()) {
     auto mode = state->getDimensionMode();
      if (mode == Project::States::DimensionMode::BOUNDED || mode == Project::States::DimensionMode::MAPPED) {
        mapRect = state->getMapRect();
        bounded = mapRect.w > 0 && mapRect.h > 0;
      }
    }

    if (bounded) {
      SDL_Rect chunkRect{
        static_cast<int>(cx * chunkSize.x),
        static_cast<int>(cy * chunkSize.y),
        static_cast<int>(chunkSize.x),
        static_cast<int>(chunkSize.y)
      };
      if (!SDL_HasIntersection(&chunkRect, &mapRect)) {
        return;
      }
    }

    float startX = cx * chunkSize.x;
    float startY = cy * chunkSize.y;
    float endX = startX + chunkSize.x;
    float endY = startY + chunkSize.y;

    if (bounded) {
      startX = std::max(startX, static_cast<float>(mapRect.x));
      startY = std::max(startY, static_cast<float>(mapRect.y));
      endX = std::min(endX, static_cast<float>(mapRect.x + mapRect.w));
      endY = std::min(endY, static_cast<float>(mapRect.y + mapRect.h));
      if (startX >= endX || startY >= endY) {
        return;
      }
    }

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
    
    size_t total = manager.getEntityCount();
    size_t available = maxSeededEntities > total ? maxSeededEntities - total : 0;
    size_t chunkCount = std::min({count, maxEntitiesPerChunk, available});
    if (chunkCount == 0) return;
    
    std::uniform_real_distribution<float> posX(startX, endX);
    std::uniform_real_distribution<float> posY(startY, endY);
    std::uniform_int_distribution<size_t> templateIndex(0, entityTemplates.empty() ? 0 : entityTemplates.size() - 1);
    
    for (size_t i = 0; i < chunkCount; ++i) {
      if (entityTemplates.empty()) break;
      std::string tmpl = entityTemplates[templateIndex(localRng)];
      float ex = posX(localRng);
      float ey = posY(localRng);

      spawnQueue.push([this, tmpl, ex, ey, k, cx, cy]() {
        if (manager.getEntityCount() >= maxSeededEntities) return;

        bool boundedLocal = false;
        SDL_Rect mapLocal{0,0,0,0};
        if (auto* state = manager.getGameState()) {
          auto mode = state->getDimensionMode();
          if (mode == Project::States::DimensionMode::BOUNDED ||
              mode == Project::States::DimensionMode::MAPPED) {
            mapLocal = state->getMapRect();
            boundedLocal = mapLocal.w > 0 && mapLocal.h > 0;
          }
        }

        EntitiesFactory::EntityPtr entity = factory.cloneEntity(tmpl);
        if (!entity) return;

        float x = ex;
        float y = ey;
        entity->getLuaStateWrapper().setGlobalNumber(Keys::X, x);
        entity->getLuaStateWrapper().setGlobalNumber(Keys::Y, y);
        entity->initialize();
        entity->setPosition(x, y);
        
        float leftOffset = 0.0f, rightOffset = 0.0f;
        float topOffset = 0.0f, bottomOffset = 0.0f;
        float w = 0.0f, h = 0.0f;
        if (auto* box = entity->getBoundingBoxComponent()) {
          box->setEntityPosition(static_cast<int>(x), static_cast<int>(y));
          const auto& boxes = box->getBoxes();
          if (!boxes.empty()) {
            float minRelX = boxes.front().x - x;
            float maxRelX = boxes.front().x + boxes.front().w - x;
            float minRelY = boxes.front().y - y;
            float maxRelY = boxes.front().y + boxes.front().h - y;
            for (const auto& b : boxes) {
              minRelX = std::min(minRelX, b.x - x);
              maxRelX = std::max(maxRelX, b.x + b.w - x);
              minRelY = std::min(minRelY, b.y - y);
              maxRelY = std::max(maxRelY, b.y + b.h - y);
            }
            leftOffset = minRelX;
            rightOffset = maxRelX;
            topOffset = minRelY;
            bottomOffset = maxRelY;
            w = rightOffset - leftOffset;
            h = bottomOffset - topOffset;
          }
        } else if (auto* gfx = entity->getGraphicsComponent()) {
          w = static_cast<float>(gfx->getWidth());
          h = static_cast<float>(gfx->getHeight());
          rightOffset = w;
          bottomOffset = h;
        }

        float chunkMinX = cx * chunkSize.x;
        float chunkMinY = cy * chunkSize.y;
        float chunkMaxX = chunkMinX + chunkSize.x;
        float chunkMaxY = chunkMinY + chunkSize.y;

        float minX = chunkMinX - leftOffset;
        float maxX = chunkMaxX - rightOffset;
        float minY = chunkMinY - topOffset;
        float maxY = chunkMaxY - bottomOffset;
        
        if (boundedLocal) {
          minX = std::max(minX, static_cast<float>(mapLocal.x) - leftOffset);
          maxX = std::min(maxX, static_cast<float>(mapLocal.x + mapLocal.w) - rightOffset);
          minY = std::max(minY, static_cast<float>(mapLocal.y) - topOffset);
          maxY = std::min(maxY, static_cast<float>(mapLocal.y + mapLocal.h) - bottomOffset);
        }

        if (maxX < minX || maxY < minY) return;

        x = std::clamp(x, minX, maxX);
        y = std::clamp(y, minY, maxY);

        entity->setPosition(x, y);
        entity->getLuaStateWrapper().setGlobalNumber(Keys::X, x);
        entity->getLuaStateWrapper().setGlobalNumber(Keys::Y, y);
        if (auto* box = entity->getBoundingBoxComponent()) {
          box->setEntityPosition(static_cast<int>(x), static_cast<int>(y));
        }

        SDL_FRect newRect{x + leftOffset, y + topOffset, w, h};
        bool overlap = false;
        for (const auto& [existingKey, existingChunk] : chunks) {
          int ecx = static_cast<int>(existingKey >> Constants::BIT_32);
          int ecy = static_cast<int>(static_cast<unsigned int>(existingKey));
          if (std::abs(ecx - cx) > 1 || std::abs(ecy - cy) > 1) continue;
          for (const std::string& eid : existingChunk.ids) {
            auto ent = manager.getEntity(eid);
            if (!ent) continue;
            SDL_FRect r{ent->getX(), ent->getY(), 0.0f, 0.0f};
            if (auto* gfx = ent->getGraphicsComponent()) {
              r.w = gfx->getWidth();
              r.h = gfx->getHeight();
            } else if (auto* box = ent->getBoundingBoxComponent()) {
              const auto& boxes = box->getBoxes();
              if (!boxes.empty()) r = boxes.front();
            }
            if (SDL_HasIntersectionF(&newRect, &r)) { overlap = true; break; }
          }
          if (overlap) break;
        }
        if (overlap) return;

        auto it = chunks.find(k);
        if (it == chunks.end()) return;
        size_t newId = idCounter.fetch_add(1);
        std::string id = tmpl + std::string(Constants::SEED) + std::to_string(newId);
        std::shared_ptr<Entity> shared = std::move(entity);
        std::string finalId = manager.addEntity(shared, id);
        it->second.ids.push_back(finalId);
      });
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
