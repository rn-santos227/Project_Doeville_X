#include "EntitiesManager.h"

namespace Project::Entities {
  void EntitiesManager::addEntity(const std::string& id, std::shared_ptr<Entity> entity) {
    std::lock_guard<std::mutex> lock(entitiesMutex);
    activeEntities[id] = std::move(entity);
  }

   void EntitiesManager::removeEntity(const std::string& id) {
    std::lock_guard<std::mutex> lock(entitiesMutex);
   }
}