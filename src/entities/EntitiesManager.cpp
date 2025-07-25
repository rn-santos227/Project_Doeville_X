#include "EntitiesManager.h"

#include <algorithm>

#include <SDL.h>

#include "bindings/LuaBindings.h"
#include "components/bounding_box_component/BoundingBoxComponent.h"
#include "components/graphics_component/GraphicsComponent.h"
#include "components/motion_component/MotionComponent.h"
#include "components/text_component/TextComponent.h"
#include "handlers/camera/CameraHandler.h"
#include "libraries/categories/ComponentCategories.h"
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
    entityList.push_back(objects[finalId]);
    std::string group = objects[finalId]->getGroup();
    if (!group.empty()) {
      entityGroups[group].push_back(finalId);
    }

    return finalId;
  }

  void EntitiesManager::removeEntity(const std::string& id) {
    auto it = std::find_if(entityList.begin(), entityList.end(), [&](const std::shared_ptr<Entity>& e){
      return e && e->getEntityID() == id;
    });
    if (it != entityList.end()) {
      entityList.erase(it);
    }
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
    entityList.clear();
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
    std::lock_guard<std::mutex> lock(managerMutex);
    for (auto& entity : entityList) {
      if (entity) entity->update(deltaTime);
    }
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
    for (auto& obj : entityList) {
      if (useCull) {
        SDL_Rect rect{0,0,0,0};
        bool hasRect = false;

        auto* bbox = obj->getBoundingBoxComponent();
        if (bbox) {
          const auto& boxes = bbox->getBoxes();
          if (!boxes.empty()) {
            rect = boxes.front();
            for (size_t i = 1; i < boxes.size(); ++i) {
              const SDL_Rect& b = boxes[i];
              int minX = std::min(rect.x, b.x);
              int minY = std::min(rect.y, b.y);
              int maxX = std::max(rect.x + rect.w, b.x + b.w);
              int maxY = std::max(rect.y + rect.h, b.y + b.h);
              rect.x = minX;
              rect.y = minY;
              rect.w = maxX - minX;
              rect.h = maxY - minY;
            }
            hasRect = true;
          }
        }

        if (!hasRect) {
          auto* gfx = dynamic_cast<Project::Components::GraphicsComponent*>(
            obj->getComponent(Project::Libraries::Categories::Components::GRAPHICS));
          if (gfx) {
            rect = gfx->getRect();
            hasRect = true;
          }
        }

        if (hasRect && !SDL_HasIntersection(&rect, &cullRect)) {
          continue;
        }
      }
      obj->render();
    }
  }

  void EntitiesManager::reset() {
    std::lock_guard<std::mutex> lock(managerMutex);
    initialized = false;
    
    objects.clear();
    entityList.clear();
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
        removeEntity(id);
      }
      entityGroups.erase(it);
    }
  }

  void EntitiesManager::registerEntityLuaFunctions(Entity* entity) {
    if (!entity) return;
    std::string path = entity->getScriptPath();
    std::string code;
    if (!path.empty()) {
      std::ifstream in(path);
      if (in) {
        std::ostringstream ss;
        ss << in.rdbuf();
        code = ss.str();
      }
    }

    auto contains = [&](const std::string& name) {
      return code.find(name) != std::string::npos;
    };

    if (contains(Keys::LUA_GET_ENTITY_SPEED)) {
      entity->registerLuaFunction(Keys::LUA_GET_ENTITY_SPEED, LuaBindings::lua_getEntitySpeed, this);
    }
    
    if (contains(Keys::LUA_SET_ENTITY_TEXT)) {
      entity->registerLuaFunction(Keys::LUA_SET_ENTITY_TEXT, LuaBindings::lua_setEntityText, this);
    }

    if (contains(Keys::LUA_DESTROY_ENTITY)) {
      entity->registerLuaFunction(Keys::LUA_DESTROY_ENTITY, LuaBindings::lua_destroyEntity, this);
    }

    if (contains(Keys::LUA_SET_TIMER_ACTIVE)) {
      entity->registerLuaFunction(Keys::LUA_SET_TIMER_ACTIVE, LuaBindings::lua_setTimerActive, this);
    }

    if (contains(Keys::LUA_ADD_NUMERIC_VALUE)) {
      entity->registerLuaFunction(Keys::LUA_ADD_NUMERIC_VALUE, LuaBindings::lua_addNumericValue, this);
    }

    if (contains(Keys::LUA_SET_NUMERIC_VALUE)) {
      entity->registerLuaFunction(Keys::LUA_SET_NUMERIC_VALUE, LuaBindings::lua_setNumericValue, this);
    }

    if (contains(Keys::LUA_GET_NUMERIC_VALUE)) {
      entity->registerLuaFunction(Keys::LUA_GET_NUMERIC_VALUE, LuaBindings::lua_getNumericValue, this);
    }

    if (contains(Keys::LUA_STOP_TIMER)) {
      entity->registerLuaFunction(Keys::LUA_STOP_TIMER, LuaBindings::lua_stopTimer, this);
    }

    if (contains(Keys::LUA_BRAKE_ENTITY)) {
      entity->registerLuaFunction(Keys::LUA_BRAKE_ENTITY, LuaBindings::lua_brakeEntity, this);
    }

    if (gameState && contains(Keys::LUA_SPAWN_ENTITY)) {
      entity->registerLuaFunction(Keys::LUA_SPAWN_ENTITY, LuaBindings::lua_spawnEntity, gameState);
    }
  }

  size_t EntitiesManager::getEntityCount() const {
    return ObjectsManager<Entity>::count();
  }
}
