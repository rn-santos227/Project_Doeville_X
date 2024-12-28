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

    optimizeEntities();
    for (const auto& [id, entity] : activeEntities) {
      cachedEntities[id] = entity;
    }
    activeEntities.clear();
  }

  void EntitiesManager::optimizeEntities() {
    std::lock_guard<std::mutex> lock(entitiesMutex);
    cachedEntities.clear();
  }

  void EntitiesManager::initialize() {
    if (!initialized) {
      initialized = true;
      for (const auto& [id, entity] : activeEntities) {
        entity->initialize();
      }
    }
  }

  void EntitiesManager::update(float deltaTime) {
    std::lock_guard<std::mutex> lock(entitiesMutex);
    for (const auto& [id, entity] : activeEntities) {
      entity->update(deltaTime);
    }
  }

  void EntitiesManager::render() {
    std::lock_guard<std::mutex> lock(entitiesMutex);
    for (const auto& [id, entity] : activeEntities) {
      entity->render();
    }
  }

  void EntitiesManager::reset() {
    std::lock_guard<std::mutex> lock(entitiesMutex);
  }
}