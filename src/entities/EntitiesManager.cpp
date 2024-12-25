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
      explicit EntitiesManager(LogsManager& logsManager);
      ~EntitiesManager();

      void addEntity(const std::string& id, std::shared_ptr<Entity> entity);
      void removeEntity(const std::string& id);

      void unloadSceneEntities();
      void optimizeEntities();

      void update(float deltaTime);
      
    private:
      LogsManager& logsManager;

      std::unordered_map<std::string, std::shared_ptr<Entity>> activeEntities;
      std::unordered_map<std::string, std::shared_ptr<Entity>> cachedEntities;

      std::mutex entitiesMutex;
  };
}

#endif