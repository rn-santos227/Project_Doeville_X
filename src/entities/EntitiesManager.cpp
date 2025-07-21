#include "EntitiesManager.h"

#include <algorithm>

#include <SDL.h>

#include "bindings/LuaBindings.h"
#include "components/bounding_box_component/BoundingBoxComponent.h"
#include "components/graphics_component/GraphicsComponent.h"
#include "components/motion_component/MotionComponent.h"
#include "components/text_component/TextComponent.h"
#include "handlers/camera/CameraHandler.h"
#include "libraries/constants/Constants.h"
#include "libraries/keys/Keys.h"
#include "states/GameState.h"

namespace Project::Entities {
  using Project::Helpers::ObjectsManager;

  namespace Constants = Project::Libraries::Constants;
  namespace Keys = Project::Libraries::Keys;
  namespace LuaBindings = Project::Bindings::LuaBindings;

  std::string EntitiesManager::addEntity(std::shared_ptr<Entity> entity, const std::string& id) {
    if (!entity) return Constants::EMPTY_STRING;

    entity->setEntitiesManager(this);
    registerEntityLuaFunctions(entity.get());
    
    std::string baseId = id.empty() ? entity->getEntityName() : id;
    if (baseId.empty()) baseId = Constants::ENTITY;

    std::string finalId = baseId;

    if (objects.find(finalId) != objects.end()) {
      int& counter = idCounters[baseId];
      do {
        finalId = baseId + "_" + std::to_string(counter++);
      } while (objects.find(finalId) != objects.end());
    }
    
    entity->setEntityID(finalId);
    add(finalId, std::move(entity));
    std::string group = objects[finalId]->getGroup();
    if (!group.empty()) {
      entityGroups[group].push_back(finalId);
    }

    return finalId;
  }

  void EntitiesManager::removeEntity(const std::string& id) {
    remove(id);
    for (auto& [group, ids] : entityGroups) {
      ids.erase(std::remove(ids.begin(), ids.end(), id), ids.end());
    }
  }

  bool EntitiesManager::hasEntity(const std::string& id) {
    std::lock_guard<std::mutex> lock(managerMutex);
    return objects.find(id) != objects.end();
  }

  std::shared_ptr<Entity> EntitiesManager::getEntity(const std::string& id) {
    auto it = objects.find(id);
    if (it != objects.end()) return it->second;
    auto cit = cachedEntities.find(id);
    if (cit != cachedEntities.end()) return cit->second;
    return nullptr;
  }

  void EntitiesManager::unloadSceneEntities() {
    std::lock_guard<std::mutex> lock(managerMutex);

    optimizeEntities();
    for (const auto& [id, entity] : objects) {
      cachedEntities[id] = entity;
    }
    objects.clear();
    entityGroups.clear();
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
    SDL_Rect cullRect{0,0,0,0};
    bool useCull = false;
    auto* camHandler = Project::Components::GraphicsComponent::getCameraHandler();
    if (camHandler) {
      cullRect = camHandler->getCullingRect();
      useCull = true;
    }

    std::lock_guard<std::mutex> lock(managerMutex);
    for (auto& [id, obj] : objects) {
      if (useCull) {
        auto* gfx = dynamic_cast<Project::Components::GraphicsComponent*>(
          obj->getComponent(Project::Libraries::Categories::Components::GRAPHICS));
        if (gfx) {
          SDL_Rect rect = gfx->getRect();
          if (!SDL_HasIntersection(&rect, &cullRect)) {
            continue;
          }
        }
      }
      obj->render();
    }
  }

  void EntitiesManager::reset() {
    std::lock_guard<std::mutex> lock(managerMutex);
    initialized = false;
    
    objects.clear();
    idCounters.clear();
    entityGroups.clear();
  }

  std::vector<std::shared_ptr<Entity>> EntitiesManager::getEntitiesByGroup(const std::string& group) {
    std::vector<std::shared_ptr<Entity>> result;
    auto it = entityGroups.find(group);
    if (it != entityGroups.end()) {
      for (const auto& id : it->second) {
        auto ent = getEntity(id);
        if (ent) result.push_back(ent);
      }
    }
    return result;
  }

  void EntitiesManager::clearGroup(const std::string& group) {
    auto it = entityGroups.find(group);
    if (it != entityGroups.end()) {
      for (const auto& id : it->second) {
        remove(id);
      }
      entityGroups.erase(it);
    }
  }

  void EntitiesManager::registerEntityLuaFunctions(Entity* entity) {
    if (!entity) return;
    entity->registerLuaFunction(Keys::LUA_GET_ENTITY_SPEED, LuaBindings::lua_getEntitySpeed, this);
    entity->registerLuaFunction(Keys::LUA_SET_ENTITY_TEXT, LuaBindings::lua_setEntityText, this);
    entity->registerLuaFunction(Keys::LUA_DESTROY_ENTITY, LuaBindings::lua_destroyEntity, this);
    entity->registerLuaFunction(Keys::LUA_SET_TIMER_ACTIVE, LuaBindings::lua_setTimerActive, this);
    entity->registerLuaFunction(Keys::LUA_STOP_TIMER, LuaBindings::lua_stopTimer, this);
    entity->registerLuaFunction(Keys::LUA_BRAKE_ENTITY, LuaBindings::lua_brakeEntity, this);
    if (gameState) {
      entity->registerLuaFunction(Keys::LUA_SPAWN_ENTITY, LuaBindings::lua_spawnEntity, gameState);
    }
  }

  size_t EntitiesManager::getEntityCount() const {
    return ObjectsManager<Entity>::count();
  }
}
