#include "EntitiesManager.h"

#include "components/bounding_box_component/BoundingBoxComponent.h"
#include "components/graphics_component/GraphicsComponent.h"
#include "components/text_component/TextComponent.h"
#include "handlers/camera/CameraHandler.h"

#include <SDL.h>

namespace Project::Entities {
  using Project::Helpers::ObjectsManager;

  void EntitiesManager::addEntity(const std::string& id, std::shared_ptr<Entity> entity) {
    add(id, std::move(entity));
  }

  void EntitiesManager::removeEntity(const std::string& id) {
    remove(id);
  }

  bool EntitiesManager::hasEntity(const std::string& id) {
    std::lock_guard<std::mutex> lock(managerMutex);
    return objects.find(id) != objects.end();
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
    ObjectsManager<Entity>::update(deltaTime);
  }

  void EntitiesManager::render() {
    ObjectsManager<Entity>::render();
  }

  void EntitiesManager::reset() {
    std::lock_guard<std::mutex> lock(managerMutex);
    objects.clear();
    initialized = false;
  }
}
