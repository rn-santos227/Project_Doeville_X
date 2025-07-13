#include "EntitiesManager.h"

#include <algorithm>

#include <SDL.h>

#include "components/bounding_box_component/BoundingBoxComponent.h"
#include "components/graphics_component/GraphicsComponent.h"
#include "components/motion_component/MotionComponent.h"
#include "components/text_component/TextComponent.h"
#include "handlers/camera/CameraHandler.h"
#include "libraries/keys/Keys.h"
#include "states/GameState.h"

namespace Project::Entities {
  using Project::Helpers::ObjectsManager;

  namespace Keys = Project::Libraries::Keys;

  void EntitiesManager::addEntity(const std::string& id, std::shared_ptr<Entity> entity) {
    if (!entity) return;
    entity->setEntitiesManager(this);
    registerEntityLuaFunctions(entity.get());
    
    std::string finalId = id;
    if (objects.find(finalId) != objects.end()) {
      int& counter = idCounters[id];
      finalId = id + "_" + std::to_string(counter++);
    }
    
    add(finalId, std::move(entity));
    std::string group = objects[finalId]->getGroup();
    if (!group.empty()) {
      entityGroups[group].push_back(finalId);
    }
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
    ObjectsManager<Entity>::render();
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
    entity->registerLuaFunction(Keys::LUA_GET_ENTITY_SPEED, lua_getEntitySpeed, this);
    entity->registerLuaFunction(Keys::LUA_SET_ENTITY_TEXT, lua_setEntityText, this);
  }

  int EntitiesManager::lua_getEntitySpeed(lua_State* L) {
    EntitiesManager* manager = static_cast<EntitiesManager*>(lua_touserdata(L, lua_upvalueindex(1)));
    const char* name = luaL_checkstring(L, 1);
    if (!manager || !name) {
      lua_pushnil(L);
      return 1;
    }
    auto entity = manager->getEntity(name);
    if (!entity && manager->getGameState()) {
      entity = manager->getGameState()->findEntity(name);
    }

    if (!entity) {
      lua_pushnil(L);
      return 1;
    }
    auto* motion = dynamic_cast<Project::Components::MotionComponent*>(entity->getComponent("MotionComponent"));
    if (!motion) {
      lua_pushnil(L);
      return 1;
    }
    lua_pushnumber(L, motion->getCurrentSpeed());
    return 1;
  }

  int EntitiesManager::lua_setEntityText(lua_State* L) {
    EntitiesManager* manager = static_cast<EntitiesManager*>(lua_touserdata(L, lua_upvalueindex(1)));
    const char* name = luaL_checkstring(L, 1);
    const char* text = luaL_checkstring(L, 2);
    if (!manager || !name || !text) {
      return 0;
    }
    auto entity = manager->getEntity(name);
    if (!entity && manager->getGameState()) {
      entity = manager->getGameState()->findEntity(name);
    }

    if (!entity) return 0;
    auto* textComp = dynamic_cast<Project::Components::TextComponent*>(entity->getComponent("TextComponent"));
    if (!textComp) return 0;
    textComp->setText(text);
    return 0;
  }
}
