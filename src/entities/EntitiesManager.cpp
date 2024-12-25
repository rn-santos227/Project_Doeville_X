#include "EntitiesManager.h"

namespace Project::Entities {
  void EntitiesManager::addEntity(const std::string& id, std::shared_ptr<Entity> entity) {
    std::lock_guard<std::mutex> lock(entitiesMutex);
    activeEntities[id] = std::move(entity);
  }

   void EntitiesManager::removeEntity(const std::string& id) {
    std::lock_guard<std::mutex> lock(entitiesMutex);

    if (activeEntities.find(id) != activeEntities.end()) {
      activeEntities.erase(id);
    }
   }

  void EntitiesManager::unloadSceneEntities() {
    std::lock_guard<std::mutex> lock(entitiesMutex);

    for (const auto& [id, entity] : activeEntities) {

    }
  }
}