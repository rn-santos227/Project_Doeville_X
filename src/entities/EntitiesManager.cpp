#include "EntitiesManager.h"

namespace Project::Entities {
  void EntitiesManager::addEntity(const std::string& id, std::shared_ptr<Entity> entity) {
    std::lock_guard<std::mutex> lock(entitiesMutex);
  }
}