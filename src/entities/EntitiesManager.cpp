#include "EntitiesManager.h"
#include "EntityAttribute.h"

#include <algorithm>
#include <fstream>
#include <sstream>

#include <SDL.h>

#include "bindings/LuaBindings.h"
#include "components/bounding_box_component/BoundingBoxComponent.h"
#include "components/graphics_component/GraphicsComponent.h"
#include "components/motion_component/MotionComponent.h"
#include "components/physics_component/PhysicsComponent.h"
#include "components/text_component/TextComponent.h"
#include "handlers/camera/CameraHandler.h"
#include "libraries/categories/ComponentCategories.h"
#include "libraries/constants/Constants.h"
#include "libraries/keys/Keys.h"
#include "states/GameState.h"
#include "utilities/binary_cache/BinaryFileCache.h"

namespace Project::Entities {
  using Project::Helpers::ObjectsManager;

  namespace Constants = Project::Libraries::Constants;
  namespace Keys = Project::Libraries::Keys;
  namespace LuaBindings = Project::Bindings::LuaBindings;

  EntitiesManager::EntitiesManager()
    : persistentFunctionCache(Constants::SCRIPT_FUNCTION_CACHE_FILE) {
    entityList.reserve(Constants::MAX_MEMORY_SPACE);
  }

  EntitiesManager::~EntitiesManager() {
    persistentFunctionCache.save();
  }

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
    objects[finalId]->getLuaStateWrapper().setGlobalString(Keys::ID, finalId);
    entityList.push_back(objects[finalId]);
    entityIndices[finalId] = entityList.size() - 1;
    
    auto& entRef = objects[finalId];
    if (entRef) {
      bool hasPhys = false;
      Project::Components::BoundingBoxComponent* box = nullptr;
      for (const std::string& compName : entRef->listComponentNames()) {
        auto* comp = entRef->getComponent(compName);
        if (!comp) continue;
        if (auto* motion = dynamic_cast<Project::Components::MotionComponent*>(comp)) {
          motionSystem.add(motion);
        } else if (auto* phys = dynamic_cast<Project::Components::PhysicsComponent*>(comp)) {
          physicsSystem.add(phys);
          hasPhys = true;
        } else if (auto* gfx = dynamic_cast<Project::Components::GraphicsComponent*>(comp)) {
          renderSystem.add(gfx);
        } else if (auto* bbox = dynamic_cast<Project::Components::BoundingBoxComponent*>(comp)) {
          box = bbox;
        }
      }

      if (box && !hasPhys) {
        physicsSystem.addStaticCollider(box);
      }
    }

    std::string group = objects[finalId]->getGroup();
    if (!group.empty()) {
      entityGroups[group].push_back(finalId);
    }

    return finalId;
  }

  void EntitiesManager::removeEntity(const std::string& id) {
    auto idxIt = entityIndices.find(id);
    if (idxIt != entityIndices.end() && idxIt->second < entityList.size()) {
      size_t index = idxIt->second;
      auto ent = entityList[index];
      if (ent) {
        for (const std::string& compName : ent->listComponentNames()) {
          auto* comp = ent->getComponent(compName);
          if (!comp) continue;
          if (auto* motion = dynamic_cast<Project::Components::MotionComponent*>(comp)) {
            motionSystem.remove(motion);
          } else if (auto* phys = dynamic_cast<Project::Components::PhysicsComponent*>(comp)) {
            physicsSystem.remove(phys);
          } else if (auto* gfx = dynamic_cast<Project::Components::GraphicsComponent*>(comp)) {
            renderSystem.remove(gfx);
          }
        }
      }

      size_t lastIndex = entityList.size() - 1;
      if (index != lastIndex) {
        entityList[index] = entityList[lastIndex];
        if (entityList[index]) {
          entityIndices[entityList[index]->getEntityID()] = index;
        }
      }
      entityList.pop_back();
      entityIndices.erase(idxIt);
    }
  }

  bool EntitiesManager::hasEntity(const std::string& id) {
    std::lock_guard<std::recursive_mutex> lock(managerMutex);
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
    std::lock_guard<std::recursive_mutex> lock(managerMutex);

    optimizeEntities();
    for (const auto& [id, entity] : objects) {
      cachedEntities[id] = entity;
    }
    objects.clear();
    entityList.clear();
    entityIndices.clear();
    entityGroups.clear();
    motionSystem.clear();
    physicsSystem.clear();
    renderSystem.clear();
  }

  void EntitiesManager::optimizeEntities() {
    std::lock_guard<std::recursive_mutex> lock(managerMutex);
    cachedEntities.clear();
    cachedEntities.rehash(0);

    entityList.shrink_to_fit();
    entityIndices.clear();
    entityIndices.rehash(0);

    for (auto& [group, ids] : entityGroups) {
      ids.shrink_to_fit();
    }
    entityGroups.rehash(0);

    for (auto& [path, funcs] : scriptFunctionCache) {
      funcs.shrink_to_fit();
    }
    scriptFunctionCache.rehash(0);

    idCounters.rehash(0);
  }

  void EntitiesManager::initialize() {
    if (!initialized) {
      initialized = true;
      for (const auto& [id, entity] : objects) {
       if (entity && !entity->hasAttribute(EntityAttribute::DEFERRED_INIT)) {
          entity->initialize();
        }
      }
    }
  }

  void EntitiesManager::update(float deltaTime) {
    std::lock_guard<std::recursive_mutex> lock(managerMutex);
    std::vector<std::shared_ptr<Entity>> high;
    std::vector<std::shared_ptr<Entity>> normal;
    std::vector<std::shared_ptr<Entity>> low;

    for (auto& entity : entityList) {
      if (!entity) continue;
      if (entity->hasAttribute(EntityAttribute::HIGH_PRIORITY)) {
        high.push_back(entity);
      } else if (entity->hasAttribute(EntityAttribute::LOW_PRIORITY)) {
        low.push_back(entity);
      } else {
        normal.push_back(entity);
      }
    }

    auto updateEntity = [&](std::shared_ptr<Entity>& ent) {
      if (!ent) return;
      if (ent->isActive() || ent->hasAttribute(EntityAttribute::PERMANENT)) {
        ent->update(deltaTime);
      }
    };

    for (auto& ent : high) updateEntity(ent);
    for (auto& ent : normal) updateEntity(ent);
    for (auto& ent : low) updateEntity(ent);

    std::vector<std::string> toRemove;
    for (const auto& ent : entityList) {
      if (ent && ent->hasAttribute(EntityAttribute::VOLATILE) && !isEntityInCamera(ent)) {
        toRemove.push_back(ent->getEntityID());
      }
    }
    for (const auto& id : toRemove) {
      removeEntity(id);
      remove(id);
    }

    motionSystem.update(deltaTime);
    physicsSystem.update(deltaTime);
    renderSystem.update(deltaTime);
  }

  void EntitiesManager::render() {
    renderSystem.render();
    std::vector<std::shared_ptr<Entity>> high;
    std::vector<std::shared_ptr<Entity>> normal;
    std::vector<std::shared_ptr<Entity>> low;

    for (auto& obj : entityList) {
      if (!obj) continue;
      if (obj->hasAttribute(EntityAttribute::HIGH_PRIORITY)) high.push_back(obj);
      else if (obj->hasAttribute(EntityAttribute::LOW_PRIORITY)) low.push_back(obj);
      else normal.push_back(obj);
    }

    auto renderEntity = [&](std::shared_ptr<Entity>& ent) {
      if (!ent) return;
      if (ent->isActive() || ent->hasAttribute(EntityAttribute::PERMANENT) || ent->hasAttribute(EntityAttribute::PERSISTENT)) {
        ent->render();
      }
    };
    for (auto& e : high) renderEntity(e);
    for (auto& e : normal) renderEntity(e);
    for (auto& e : low) renderEntity(e);
  }

  void EntitiesManager::reset() {
    std::lock_guard<std::recursive_mutex> lock(managerMutex);
    initialized = false;
    
    objects.clear();
    entityList.clear();
    entityGroups.clear();
    entityIndices.clear();
    idCounters.clear();
    motionSystem.clear();
    physicsSystem.clear();
    renderSystem.clear();
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

  size_t EntitiesManager::getEntityCount() const {
    return ObjectsManager<Entity>::count();
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
    const std::string& path = entity->getScriptPath();
    std::vector<std::string> functions;

    if (!path.empty()) {
      auto cacheIt = scriptFunctionCache.find(path);
      if (cacheIt == scriptFunctionCache.end()) {
        std::vector<char> raw;
        if (persistentFunctionCache.getData(path, raw)) {
          std::string content(raw.begin(), raw.end());
          std::istringstream iss(content);
          std::string func;
          while (std::getline(iss, func)) {
            if (!func.empty()) functions.push_back(func);
          }
        } else {
          std::string code;
          std::ifstream in(path);
          if (in) {
            std::ostringstream ss;
            ss << in.rdbuf();
            code = ss.str();
          }

          auto contains = [&](const std::string& name) {
            return code.find(name) != std::string::npos;
          };

          auto record = [&](const std::string& name) {
            if (contains(name)) functions.push_back(name);
          };

          record(Keys::LUA_GET_ENTITY_DETAILS);
          record(Keys::LUA_GET_ENTITY_SPEED);
          record(Keys::LUA_SET_ENTITY_TEXT);
          record(Keys::LUA_SPAWN_ENTITY);
          record(Keys::LUA_DESTROY_ENTITY);
          record(Keys::LUA_DESTROY_ENTITIES);
          record(Keys::LUA_SET_TIMER_ACTIVE);
          record(Keys::LUA_ADD_NUMERIC_VALUE);
          record(Keys::LUA_SET_NUMERIC_VALUE);
          record(Keys::LUA_GET_NUMERIC_VALUE);
          record(Keys::LUA_STOP_TIMER);
          record(Keys::LUA_BRAKE_ENTITY);
          record(Keys::LUA_IS_ACTION_PRESSED);
          record(Keys::LUA_EXIT_GAME);

          std::string serialized;
          for (const auto& f : functions) serialized += f + "\n";
          persistentFunctionCache.setData(path, std::vector<char>(serialized.begin(), serialized.end()));
        }

        scriptFunctionCache[path] = functions;
      } else {
        functions = cacheIt->second;
      }

      for (const auto& func : functions) {
        if (func == Keys::LUA_GET_ENTITY_DETAILS) {
          entity->registerLuaFunction(func, LuaBindings::lua_getEntityDetails, this);
        } else if (func == Keys::LUA_GET_ENTITY_SPEED) {
          entity->registerLuaFunction(func, LuaBindings::lua_getEntitySpeed, this);
        } else if (func == Keys::LUA_SET_ENTITY_TEXT) {
          entity->registerLuaFunction(func, LuaBindings::lua_setEntityText, this);
        } else if (func == Keys::LUA_DESTROY_ENTITY) {
          entity->registerLuaFunction(func, LuaBindings::lua_destroyEntity, this);
        } else if (func == Keys::LUA_DESTROY_ENTITIES) {
          entity->registerLuaFunction(func, LuaBindings::lua_destroyEntities, this);
        } else if (func == Keys::LUA_SET_TIMER_ACTIVE) {
          entity->registerLuaFunction(func, LuaBindings::lua_setTimerActive, this);
        } else if (func == Keys::LUA_ADD_NUMERIC_VALUE) {
          entity->registerLuaFunction(func, LuaBindings::lua_addNumericValue, this);
        } else if (func == Keys::LUA_SET_NUMERIC_VALUE) {
          entity->registerLuaFunction(func, LuaBindings::lua_setNumericValue, this);
        } else if (func == Keys::LUA_GET_NUMERIC_VALUE) {
          entity->registerLuaFunction(func, LuaBindings::lua_getNumericValue, this);
        } else if (func == Keys::LUA_STOP_TIMER) {
          entity->registerLuaFunction(func, LuaBindings::lua_stopTimer, this);
        } else if (func == Keys::LUA_BRAKE_ENTITY) {
          entity->registerLuaFunction(func, LuaBindings::lua_brakeEntity, this);
        } else if (func == Keys::LUA_IS_ACTION_PRESSED) {
          entity->registerLuaFunction(func, LuaBindings::lua_isActionPressed, this);
        } else if (func == Keys::LUA_SPAWN_ENTITY && gameState) {
          entity->registerLuaFunction(func, LuaBindings::lua_spawnEntity, gameState);
        } else if (func == Keys::LUA_EXIT_GAME && sdlManager) {
          entity->registerLuaFunction(func, LuaBindings::lua_exitGame, sdlManager);
        }
      }
    }
  }

  void EntitiesManager::clearScriptFunctionCache() {
    scriptFunctionCache.clear();
    persistentFunctionCache.load();
  }

  bool EntitiesManager::isEntityInCamera(const std::shared_ptr<Entity>& entity) const {
    if (!entity) return true;
    auto* cam = Project::Components::GraphicsComponent::getCameraHandler();
    if (!cam) return true;
    const SDL_Rect cullRect = cam->getCullingRect();

    if (auto* bbox = entity->getBoundingBoxComponent()) {
      const auto& boxes = bbox->getBoxes();
      for (const auto& box : boxes) {
        if (SDL_HasIntersection(&box, &cullRect)) return true;
      }
      return false;
    }

    if (auto* gfx = entity->getGraphicsComponent()) {
      const SDL_Rect rect = gfx->getRect();
      return SDL_HasIntersection(&rect, &cullRect);
    }

    SDL_Rect point{static_cast<int>(entity->getX()), static_cast<int>(entity->getY()), 1, 1};
    return SDL_HasIntersection(&point, &cullRect);
  }
}
