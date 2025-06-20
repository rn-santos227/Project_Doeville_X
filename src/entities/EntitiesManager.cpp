#include "EntitiesManager.h"

namespace Project::Entities {
  void EntitiesManager::addEntity(const std::string& id, std::shared_ptr<Entity> entity) {
    add(id, std::move(entity));
  }

  void EntitiesManager::removeEntity(const std::string& id) {
    remove(id);
  }

  void EntitiesManager::unloadSceneEntities() {
    std::lock_guard<std::mutex> lock(managerMutex);

    optimizeEntities();
    for (const auto& [id, entity] : objects) {
      cachedEntities[id] = entity;
    }
    objects.clear();
  }

  void EntitiesManager::optimizeEntities() {
    std::lock_guard<std::mutex> lock(managerMutex);
    cachedEntities.clear();
  }

  void EntitiesManager::initialize() {
    if (!initialized) {
      initialized = true;
      for (const auto& [id, entity] : objects) {
        entity->initialize();
      }
    }
  }

  void EntitiesManager::update(float deltaTime) {
    Manager<Entity>::update(deltaTime);
  }

  void EntitiesManager::render() {
    Manager<Entity>::render();
  }

  void EntitiesManager::reset() {
    std::lock_guard<std::mutex> lock(managerMutex);
  }
}
