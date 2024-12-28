#ifndef ENTITIES_MANAGER_H
#define ENTITIES_MANAGER_H

#include "Entity.h"

#include <unordered_map>
#include <vector>
#include <memory>
#include <string>
#include <mutex>

namespace Project::Entities {
  class EntitiesManager {
    public:
      EntitiesManager() = default;
      ~EntitiesManager() = default;

      void addEntity(const std::string& id, std::shared_ptr<Entity> entity);
      void removeEntity(const std::string& id);

      void unloadSceneEntities();
      void optimizeEntities();

      void update(float deltaTime);
      void render();

    private:
      std::unordered_map<std::string, std::shared_ptr<Entity>> activeEntities;
      std::unordered_map<std::string, std::shared_ptr<Entity>> cachedEntities;

      std::mutex entitiesMutex;
      bool initialized = false;
  };
}

#endif