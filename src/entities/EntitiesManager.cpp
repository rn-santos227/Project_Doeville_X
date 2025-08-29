#include "EntitiesManager.h"
#include "EntityAttribute.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <bitset>
#include <exception>
#include <fstream>
#include <sstream>
#include <string>

#include <SDL.h>

#include "bindings/LuaBindings.h"
#include "components/PositionableComponent.h"
#include "components/behavior_component/BehaviorComponent.h"
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
#include "utilities/profiler/CacheProfiler.h"
#include "utilities/thread/ThreadPool.h"

namespace Project::Entities {
  using Project::Utilities::LogsManager;
  using Project::Helpers::ObjectsManager;

  namespace Components = Project::Libraries::Categories::Components;
  namespace Constants = Project::Libraries::Constants;
  namespace Keys = Project::Libraries::Keys;
  namespace LuaBindings = Project::Bindings::LuaBindings;

  EntitiesManager::EntitiesManager()
    : persistentFunctionCache(Constants::SCRIPT_FUNCTION_CACHE_FILE) {
    entityList.reserve(Constants::MAX_MEMORY_SPACE);
    updateHigh.reserve(Constants::MAX_MEMORY_SPACE);
    updateNormal.reserve(Constants::MAX_MEMORY_SPACE);
    updateLow.reserve(Constants::MAX_MEMORY_SPACE);
    updateToRemove.reserve(Constants::MAX_MEMORY_SPACE);

    scheduler.addSystem(Components::BEHAVIOR, &behaviorSystem);
    scheduler.addSystem(Components::MOTION, &motionSystem);
    scheduler.addSystem(Components::PHYSICS, &physicsSystem, {Components::MOTION});
    scheduler.addSystem(Components::RENDER, &renderSystem, {Components::PHYSICS});
  }

  EntitiesManager::~EntitiesManager() {
    persistentFunctionCache.save();
  }

  std::string EntitiesManager::addEntity(std::shared_ptr<Entity> entity, const std::string& id) {
    if (!entity) return Constants::EMPTY_STRING;

    std::lock_guard<std::mutex> lock(managerMutex);
    entity->setEntitiesManager(this);

    if (gameState) {
      gameState->applyEntityScriptOverride(entity.get());
    }
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
    
    objects[finalId] = std::move(entity);
    objects[finalId]->getLuaStateWrapper().setGlobalString(Keys::ID, finalId);
    entityList.push_back(objects[finalId]);
    entityIndices[finalId] = entityList.size() - 1;
    
    auto& entRef = objects[finalId];
    if (entRef) {
      bool hasPhys = false;
      Project::Components::BoundingBoxComponent* box = nullptr;
      std::bitset<Constants::BIT_64> mask;
      for (const std::string& compName : entRef->listComponentNames()) {
        auto* comp = entRef->getComponent(compName);
        if (!comp) continue;

        auto type = comp->getType();
        auto& storage = componentArrays[type];
        storage.reserve(Constants::MAX_MEMORY_SPACE);
        storage.add(comp);
        mask.set(static_cast<size_t>(type));

        switch (type) {
          case Project::Components::ComponentType::BEHAVIOR:
            behaviorSystem.add(static_cast<Project::Components::BehaviorComponent*>(comp));
            break;
          case Project::Components::ComponentType::MOTION:
            motionSystem.add(static_cast<Project::Components::MotionComponent*>(comp));
            break;
          case Project::Components::ComponentType::PHYSICS:
            physicsSystem.add(static_cast<Project::Components::PhysicsComponent*>(comp));
            hasPhys = true;
            break;
          case Project::Components::ComponentType::GRAPHICS:
            renderSystem.add(static_cast<Project::Components::GraphicsComponent*>(comp));
            break;
          case Project::Components::ComponentType::BOUNDING_BOX:
            box = static_cast<Project::Components::BoundingBoxComponent*>(comp);
            break;
          default:
            break;
        }
      }

      if (box && !hasPhys) {
        physicsSystem.addStaticCollider(box);
      }

      size_t key = mask.to_ullong();
      size_t archIndex;
      auto lookup = archetypeLookup.find(key);
      if (lookup == archetypeLookup.end()) {
        archIndex = archetypes.size();
        archetypes.emplace_back();
        archetypeKeys.push_back(key);
        archetypeLookup[key] = archIndex;
      } else {
        archIndex = lookup->second;
      }
      archetypes[archIndex].add(entRef.get());
      entityArchetypes[finalId] = archIndex;
    }

    std::string group = objects[finalId]->getGroup();
    if (!group.empty()) {
      entityGroups[group].push_back(finalId);
    }

    if (gameState) {
      const SDL_Rect& rect = gameState->getMapRect();
      if (rect.w > 0 && rect.h > 0) {
        clampEntitiesToRect(rect);
      }
    }

    return finalId;
  }

  void EntitiesManager::removeEntity(const std::string& id) {
    auto idxIt = entityIndices.find(id);
    Entity* entRaw = nullptr;

    if (idxIt != entityIndices.end() && idxIt->second < entityList.size()) {
      size_t index = idxIt->second;
      auto ent = entityList[index];
      entRaw = ent.get();
      if (ent) {
        bool hasPhys = false;
        Project::Components::BoundingBoxComponent* box = nullptr;
        for (const std::string& compName : ent->listComponentNames()) {
          auto* comp = ent->getComponent(compName);
          if (!comp) continue;
          auto type = comp->getType();
          componentArrays[type].remove(comp);
          switch (type) {
            case Project::Components::ComponentType::BEHAVIOR:
              behaviorSystem.remove(static_cast<Project::Components::BehaviorComponent*>(comp));
              break;
            case Project::Components::ComponentType::MOTION:
              motionSystem.remove(static_cast<Project::Components::MotionComponent*>(comp));
              break;
            case Project::Components::ComponentType::PHYSICS:
              physicsSystem.remove(static_cast<Project::Components::PhysicsComponent*>(comp));
              hasPhys = true;
              break;
            case Project::Components::ComponentType::GRAPHICS:
              renderSystem.remove(static_cast<Project::Components::GraphicsComponent*>(comp));
              break;
            case Project::Components::ComponentType::BOUNDING_BOX:
              box = static_cast<Project::Components::BoundingBoxComponent*>(comp);
              break;
            default:
              break;
          }
        }

        if (box && !hasPhys) {
          physicsSystem.removeStaticCollider(box);
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

    for (auto& [group, ids] : entityGroups) {
      ids.erase(std::remove(ids.begin(), ids.end(), id), ids.end());
    }
    disposableSeenInCamera.erase(id);
    ObjectsManager<Entity>::remove(id);

    auto archIt = entityArchetypes.find(id);
    if (archIt != entityArchetypes.end()) {
      size_t idx = archIt->second;
      archetypes[idx].remove(entRaw);
      if (archetypes[idx].entities.empty()) {
        size_t last = archetypes.size() - 1;
        size_t key = archetypeKeys[idx];
        if (idx != last) {
          archetypes[idx] = std::move(archetypes[last]);
          size_t movedKey = archetypeKeys[last];
          archetypeKeys[idx] = movedKey;
          archetypeLookup[movedKey] = idx;
          for (auto* e : archetypes[idx].entities) {
            if (e) entityArchetypes[e->getEntityID()] = idx;
          }
        }
        archetypes.pop_back();
        archetypeLookup.erase(key);
        archetypeKeys.pop_back();
      }
      entityArchetypes.erase(archIt);
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

    optimizeEntitiesImpl();
    for (const auto& [id, entity] : objects) {
      cachedEntities[id] = entity;
    }
    objects.clear();
    entityList.clear();
    updateHighCount = updateNormalCount = updateLowCount = updateToRemoveCount = 0;

    entityIndices.clear();
    entityGroups.clear();
    motionSystem.clear();
    physicsSystem.clear();
    renderSystem.clear();
    disposableSeenInCamera.clear();
    componentArrays.clear();
    archetypes.clear();
    archetypeLookup.clear();
    archetypeKeys.clear();
    entityArchetypes.clear();
  }

  void EntitiesManager::optimizeEntities() {
    std::lock_guard<std::mutex> lock(managerMutex);
    optimizeEntitiesImpl();
  }

  void EntitiesManager::initialize() {
    if (!initialized) {    
    updateHigh.clear();
    updateNormal.clear();
    updateLow.clear();

    if (updateHigh.capacity() < entityList.size())
      updateHigh.reserve(entityList.size());
    if (updateNormal.capacity() < entityList.size())
      updateNormal.reserve(entityList.size());
    if (updateLow.capacity() < entityList.size())
      updateLow.reserve(entityList.size());
      initialized = true;
      for (const auto& [id, entity] : objects) {
       if (entity && !entity->hasAttribute(EntityAttribute::DEFERRED_INIT)) {
          entity->initialize();
        }
      }
    }
  }

  void EntitiesManager::update(float deltaTime) {
    Project::Utilities::CacheProfiler cacheProfiler;
    cacheProfiler.start();
    std::vector<std::string> pendingRemove;
    {
      updateHighCount = 0;
      updateNormalCount = 0;
      updateLowCount = 0;

      for (auto& entity : entityList) {
        if (!entity) continue;
        if (entity->hasAttribute(EntityAttribute::HIGH_PRIORITY)) {
          if (updateHighCount < updateHigh.size())
            updateHigh[updateHighCount] = entity;
          else
            updateHigh.push_back(entity);
          ++updateHighCount;
        } else if (entity->hasAttribute(EntityAttribute::LOW_PRIORITY)) {
          if (updateLowCount < updateLow.size())
            updateLow[updateLowCount] = entity;
          else
            updateLow.push_back(entity);
          ++updateLowCount;
        } else {
          if (updateNormalCount < updateNormal.size())
            updateNormal[updateNormalCount] = entity;
          else
            updateNormal.push_back(entity);
          ++updateNormalCount;
        }
      }

      auto updateEntity = [&](std::shared_ptr<Entity>& ent) {
        if (!ent) return;
        if (ent->isActive() || ent->hasAttribute(EntityAttribute::PERMANENT)) {
          ent->update(deltaTime);
        }
      };

      for (size_t i = 0; i < updateHighCount; ++i) updateEntity(updateHigh[i]);
      for (size_t i = 0; i < updateNormalCount; ++i) updateEntity(updateNormal[i]);
      for (size_t i = 0; i < updateLowCount; ++i) updateEntity(updateLow[i]);

      updateToRemoveCount = 0;

      for (const auto& ent : entityList) {
        if (!ent) continue;

        bool remove = false;
        bool inCamera = isEntityInCamera(ent);

        if (ent->hasAttribute(EntityAttribute::VOLATILE) && !inCamera) {
          remove = true;
        }

        if (ent->hasAttribute(EntityAttribute::DISPOSABLE)) {
          const auto& id = ent->getEntityID();
          if (inCamera) {
            disposableSeenInCamera.insert(id);
          }
          if (isEntityOutOfBounds(ent) || (!inCamera && disposableSeenInCamera.count(id))) {
            remove = true;
          }
        }

        if (remove) {
          if (updateToRemoveCount < updateToRemove.size())
            updateToRemove[updateToRemoveCount] = ent->getEntityID();
          else
            updateToRemove.push_back(ent->getEntityID());
          ++updateToRemoveCount;
        }
      }
      pendingRemove.assign(updateToRemove.begin(), updateToRemove.begin() + updateToRemoveCount);
    }

    for (size_t i = 0; i < updateToRemoveCount; ++i) {
      const auto& id = updateToRemove[i];
      removeEntity(id);
      remove(id);
    }

    scheduler.update(deltaTime);
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
    std::lock_guard<std::mutex> lock(managerMutex);
    initialized = false;
    
    objects.clear();
    updateHighCount = updateNormalCount = updateLowCount = updateToRemoveCount = 0;

    entityList.clear();
    entityGroups.clear();
    entityIndices.clear();
    idCounters.clear();
    behaviorSystem.clear();
    motionSystem.clear();
    physicsSystem.clear();
    renderSystem.clear();
    scheduler.clear();
    scheduler.addSystem(Components::BEHAVIOR, &behaviorSystem);
    scheduler.addSystem(Components::MOTION, &motionSystem);
    scheduler.addSystem(Components::PHYSICS, &physicsSystem, {Components::MOTION});
    scheduler.addSystem(Components::RENDER, &renderSystem, {Components::PHYSICS});
    componentArrays.clear();
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

  const std::vector<Project::Components::BaseComponent*>& EntitiesManager::getComponentArray(Project::Components::ComponentType type) const {
    static const std::vector<Project::Components::BaseComponent*> empty;
    auto it = componentArrays.find(type);
    return it != componentArrays.end() ? it->second.components : empty;
  }

  std::vector<Entity*> EntitiesManager::filterEntitiesByComponents(const std::vector<Project::Components::ComponentType>& types) const {
    std::bitset<Constants::BIT_64> mask;
    for (auto type : types) mask.set(static_cast<size_t>(type));
    size_t key = mask.to_ullong();
    auto it = archetypeLookup.find(key);
    if (it != archetypeLookup.end()) {
      return archetypes[it->second].entities;
    }
    std::unordered_map<Entity*, size_t> counts;
    for (auto type : types) {
      auto cit = componentArrays.find(type);
      if (cit == componentArrays.end()) return {};
      const auto& comps = cit->second;
      for (size_t i = 0; i < comps.components.size(); ++i) {
        auto* owner = comps.owners[i];
        if (owner) counts[owner]++;
      }
    }
    std::vector<Entity*> result;
    for (auto& [ent, cnt] : counts) {
      if (cnt == types.size()) result.push_back(ent);
    }
    return result;
  }

  void EntitiesManager::serializeComponentData(const std::string& path) const {
    std::ofstream out(path, std::ios::binary);
    if (!out) return;
    for (const auto& ent : entityList) {
      if (!ent) continue;
      std::ostringstream line;
      line << ent->getEntityID();
      for (const auto& name : ent->listComponentNames()) {
        if (auto* comp = ent->getComponent(name)) {
          line << ',' << static_cast<int>(comp->getType()) << ':' << comp->getClass();
        }
      }
      line << '\n';
      auto str = line.str();
      out.write(str.c_str(), str.size());
    }
  }

  void EntitiesManager::deserializeComponentData(const std::string& path) {
    std::ifstream in(path, std::ios::binary);
    if (!in) return;
    std::string line;
    while (std::getline(in, line)) {
      std::istringstream ss(line);
      std::string id;
      if (!std::getline(ss, id, ',')) continue;
      auto ent = getEntity(id);
      if (!ent) continue;
      std::string compInfo;
      while (std::getline(ss, compInfo, ',')) {
        auto pos = compInfo.find(':');
        if (pos == std::string::npos) continue;
        int typeVal = std::stoi(compInfo.substr(0, pos));
        std::string cls = compInfo.substr(pos + 1);
        for (const std::string& name : ent->listComponentNames()) {
          if (auto* comp = ent->getComponent(name)) {
            if (static_cast<int>(comp->getType()) == typeVal) {
              comp->setClass(cls);
            }
          }
        }
      }
    }
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

  void EntitiesManager::clearScriptFunctionCache() {
    scriptFunctionCache.clear();
    persistentFunctionCache.load();
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
          record(Keys::LUA_GET_COLLIDED_ENTITY);
          record(Keys::LUA_GET_ENTITY_SPEED);
          record(Keys::LUA_GET_ENTITY_VELOCITY);
          record(Keys::LUA_GET_ENTITY_ROTATION);
          record(Keys::LUA_GET_VISIBLE_ENTITIES);
          record(Keys::LUA_SET_ENTITY_TEXT);
          record(Keys::LUA_SET_COLOR);
          record(Keys::LUA_SPAWN_ENTITY);
          record(Keys::LUA_DESTROY_ENTITY);
          record(Keys::LUA_DESTROY_ENTITIES);
          record(Keys::LUA_DESTROY_SELF);
          record(Keys::LUA_SET_TIMER_ACTIVE);
          record(Keys::LUA_ADD_NUMERIC_VALUE);
          record(Keys::LUA_SUBTRACT_NUMERIC_VALUE);
          record(Keys::LUA_MULTIPLY_NUMERIC_VALUE);
          record(Keys::LUA_DIVIDE_NUMERIC_VALUE);
          record(Keys::LUA_SET_NUMERIC_VALUE);
          record(Keys::LUA_GET_NUMERIC_VALUE);
          record(Keys::LUA_STOP_TIMER);
          record(Keys::LUA_BRAKE_ENTITY);
          record(Keys::LUA_TURN_LEFT);
          record(Keys::LUA_TURN_RIGHT);
          record(Keys::LUA_IS_ACTION_PRESSED);
          record(Keys::LUA_IGNORE_COLLISIONS_WITH);
          record(Keys::LUA_CAMERA_ZOOM_IN);
          record(Keys::LUA_CAMERA_ZOOM_OUT);
          record(Keys::LUA_CAMERA_SHAKE);
          record(Keys::LUA_EXIT_GAME);

          std::string serialized;
          for (const auto& f : functions) serialized += f + "\n";
          persistentFunctionCache.setData(path, std::vector<char>(serialized.begin(), serialized.end()));
        }

        scriptFunctionCache[path] = functions;
      } else {
        functions = cacheIt->second;
      }

      auto ensure = [&](const std::string& name){
        if (std::find(functions.begin(), functions.end(), name) == functions.end()) {
          functions.push_back(name);
        }
      };
      ensure(Keys::LUA_CAMERA_ZOOM_IN);
      ensure(Keys::LUA_CAMERA_ZOOM_OUT);
      ensure(Keys::LUA_CAMERA_SHAKE);
      ensure(Keys::LUA_CAMERA_ROTATE);

      for (const auto& func : functions) {
        if (func == Keys::LUA_GET_ENTITY_DETAILS) {
          entity->registerLuaFunction(func, LuaBindings::lua_getEntityDetails, this);
        } else if (func == Keys::LUA_GET_COLLIDED_ENTITY) {
          entity->registerLuaFunction(func, LuaBindings::lua_getCollidedEntity, this);
        } else if (func == Keys::LUA_GET_ENTITY_SPEED) {
          entity->registerLuaFunction(func, LuaBindings::lua_getEntitySpeed, this);
        } else if (func == Keys::LUA_GET_ENTITY_VELOCITY) {
          entity->registerLuaFunction(func, LuaBindings::lua_getEntityVelocity, this);
        } else if (func == Keys::LUA_GET_ENTITY_ROTATION) {
          entity->registerLuaFunction(func, LuaBindings::lua_getEntityRotation, this);
        } else if (func == Keys::LUA_GET_VISIBLE_ENTITIES) {
          entity->registerLuaFunction(func, LuaBindings::lua_getVisibleEntities, this);
        } else if (func == Keys::LUA_SET_ENTITY_TEXT) {
          entity->registerLuaFunction(func, LuaBindings::lua_setEntityText, this);
        } else if (func == Keys::LUA_SET_COLOR) {
          entity->registerLuaFunction(func, LuaBindings::lua_setColor, this);
        } else if (func == Keys::LUA_DESTROY_ENTITY) {
          entity->registerLuaFunction(func, LuaBindings::lua_destroyEntity, this);
        } else if (func == Keys::LUA_DESTROY_ENTITIES) {
          entity->registerLuaFunction(func, LuaBindings::lua_destroyEntities, this);
        } else if (func == Keys::LUA_DESTROY_SELF) {
          entity->registerLuaFunction(func, LuaBindings::lua_destroySelf, this);
        } else if (func == Keys::LUA_SET_TIMER_ACTIVE) {
          entity->registerLuaFunction(func, LuaBindings::lua_setTimerActive, this);
        } else if (func == Keys::LUA_ADD_NUMERIC_VALUE) {
          entity->registerLuaFunction(func, LuaBindings::lua_addNumericValue, this);
        } else if (func == Keys::LUA_SUBTRACT_NUMERIC_VALUE) {
          entity->registerLuaFunction(func, LuaBindings::lua_subtractNumericValue, this);
        } else if (func == Keys::LUA_MULTIPLY_NUMERIC_VALUE) {
          entity->registerLuaFunction(func, LuaBindings::lua_multiplyNumericValue, this);
        } else if (func == Keys::LUA_DIVIDE_NUMERIC_VALUE) {
          entity->registerLuaFunction(func, LuaBindings::lua_divideNumericValue, this);
        } else if (func == Keys::LUA_SET_NUMERIC_VALUE) {
          entity->registerLuaFunction(func, LuaBindings::lua_setNumericValue, this);
        } else if (func == Keys::LUA_GET_NUMERIC_VALUE) {
          entity->registerLuaFunction(func, LuaBindings::lua_getNumericValue, this);
        } else if (func == Keys::LUA_STOP_TIMER) {
          entity->registerLuaFunction(func, LuaBindings::lua_stopTimer, this);
        } else if (func == Keys::LUA_BRAKE_ENTITY) {
          entity->registerLuaFunction(func, LuaBindings::lua_brakeEntity, this);
        } else if (func == Keys::LUA_TURN_LEFT) {
          entity->registerLuaFunction(func, LuaBindings::lua_turnLeft, this);
        } else if (func == Keys::LUA_TURN_RIGHT) {
          entity->registerLuaFunction(func, LuaBindings::lua_turnRight, this);
        } else if (func == Keys::LUA_IS_ACTION_PRESSED) {
          entity->registerLuaFunction(func, LuaBindings::lua_isActionPressed, this);
        } else if (func == Keys::LUA_IGNORE_COLLISIONS_WITH) {
          entity->registerLuaFunction(func, LuaBindings::lua_ignoreCollisionsWith, this);
        } else if (func == Keys::LUA_SPAWN_ENTITY && gameState) {
          entity->registerLuaFunction(func, LuaBindings::lua_spawnEntity, gameState);
        } else if (func == Keys::LUA_CAMERA_ZOOM_IN && gameState) {
          entity->registerLuaFunction(func, LuaBindings::lua_cameraZoomIn, gameState);
        } else if (func == Keys::LUA_CAMERA_ZOOM_OUT && gameState) {
          entity->registerLuaFunction(func, LuaBindings::lua_cameraZoomOut, gameState);
        } else if (func == Keys::LUA_CAMERA_ROTATE && gameState) {
          entity->registerLuaFunction(func, LuaBindings::lua_cameraRotate, gameState);
        } else if (func == Keys::LUA_CAMERA_SHAKE && gameState) {
          entity->registerLuaFunction(func, LuaBindings::lua_cameraShake, gameState);
        } else if (func == Keys::LUA_EXIT_GAME && platform) {
          entity->registerLuaFunction(func, LuaBindings::lua_exitGame, platform);
        }
      }
    }
  }

  void EntitiesManager::clampEntitiesToRect(const SDL_Rect& rect) {
    for (auto& entity : entityList) {
      if (!entity) continue;
      if (entity->hasAttribute(EntityAttribute::UNBOUNDED)) continue;

      float ex = entity->getX();
      float ey = entity->getY();

      auto [w, h] = getEntitySize(entity);

      float clampedX = std::clamp(ex, static_cast<float>(rect.x), static_cast<float>(rect.x + rect.w - w));
      float clampedY = std::clamp(ey, static_cast<float>(rect.y), static_cast<float>(rect.y + rect.h - h));

      bool clampedXChanged = clampedX != ex;
      bool clampedYChanged = clampedY != ey;

      if (clampedXChanged || clampedYChanged) {
        float newX = clampedXChanged ? clampedX : ex;
        float newY = clampedYChanged ? clampedY : ey;

        updateEntityPosition(entity, newX, newY);

        if (auto* bbox = entity->getBoundingBoxComponent()) {
          bbox->getBoxes();
        }

        if (auto* motion = dynamic_cast<Project::Components::MotionComponent*>(
              entity->getComponent(Components::MOTION_COMPONENT))) {
          float vx = motion->getVelocityX();
          float vy = motion->getVelocityY();
          if (clampedXChanged) vx = 0.0f;
          if (clampedYChanged) vy = 0.0f;
          motion->setRawVelocity(vx, vy);
        }

        if (auto* physics = entity->getPhysicsComponent()) {
          float vx = physics->getVelocityX();
          float vy = physics->getVelocityY();
          if (clampedXChanged) vx = 0.0f;
          if (clampedYChanged) vy = 0.0f;
          physics->setVelocity(vx, vy);
        }
      }
    }
  }

  void EntitiesManager::warpEntitiesAcrossRect(const SDL_Rect& rect) {
    for (auto& entity : entityList) {
      if (!entity) continue;
      if (entity->hasAttribute(EntityAttribute::UNBOUNDED)) continue;

      float ex = entity->getX();
      float ey = entity->getY();

      auto [w, h] = getEntitySize(entity);

      float newX = ex;
      float newY = ey;

      if (ex + w < rect.x) {
        newX = static_cast<float>(rect.x + rect.w);
      } else if (ex > rect.x + rect.w) {
        newX = static_cast<float>(rect.x - w);
      }

      if (ey + h < rect.y) {
        newY = static_cast<float>(rect.y + rect.h);
      } else if (ey > rect.y + rect.h) {
        newY = static_cast<float>(rect.y - h);
      }

      if (newX != ex || newY != ey) {
        updateEntityPosition(entity, newX, newY);
      }
    }
  }

  std::pair<float, float> EntitiesManager::getEntitySize(const std::shared_ptr<Entity>& entity) const {
    float gfxW = 0.0f, gfxH = 0.0f;
    float boxW = 0.0f, boxH = 0.0f;

    if (auto* gfx = entity->getGraphicsComponent()) {
      gfxW = static_cast<float>(gfx->getWidth());
      gfxH = static_cast<float>(gfx->getHeight());
    }

    if (auto* bbox = entity->getBoundingBoxComponent()) {
      const auto& boxes = bbox->getBoxes();
      if (!boxes.empty()) {
        boxW = static_cast<float>(boxes.front().w);
        boxH = static_cast<float>(boxes.front().h);
      }
    }

    float maxW = std::max(gfxW, boxW);
    float maxH = std::max(gfxH, boxH);

    return { maxW, maxH };
  }

  bool EntitiesManager::isEntityInCamera(const std::shared_ptr<Entity>& entity) const {
    if (!entity) return true;
    auto* cam = Project::Components::GraphicsComponent::getCameraHandler();
    if (!cam) return true;
    const SDL_FRect cullRect = cam->getCullingRect();

    if (auto* gfx = entity->getGraphicsComponent()) {
      SDL_FRect rect = gfx->getRect();
      
      if (rect.w <= 0.0f) {
        int w = gfx->getWidth();
        rect.w = w > 0 ? static_cast<float>(w) : 1.0f;
      }
      
      if (rect.h <= 0.0f) {
        int h = gfx->getHeight();
        rect.h = h > 0 ? static_cast<float>(h) : 1.0f;
      }

      if (SDL_HasIntersectionF(&rect, &cullRect)) return true;
    }

    if (auto* bbox = entity->getBoundingBoxComponent()) {
      const auto& boxes = bbox->getBoxes();
      for (const auto& box : boxes) {
        if (SDL_HasIntersectionF(&box, &cullRect)) return true;
      }
      return false;
    }

    SDL_FRect point{entity->getX(), entity->getY(), 1.0f, 1.0f};
    return SDL_HasIntersectionF(&point, &cullRect);
  }

  bool EntitiesManager::isEntityOutOfBounds(const std::shared_ptr<Entity>& entity) const {
    if (!entity) return false;
    if (entity->hasAttribute(EntityAttribute::UNBOUNDED)) return false;

    if (!gameState) return false;
    if (gameState->getDimensionMode() != Project::States::DimensionMode::BOUNDED)
      return false;

    const SDL_Rect& rect = gameState->getMapRect();

    auto [w, h] = getEntitySize(entity);
    float ex = entity->getX();
    float ey = entity->getY();

    if (ex <= rect.x || ey <= rect.y) return true;
    if (ex + w >= rect.x + rect.w || ey + h >= rect.y + rect.h) return true;
    return false;
  }

  void EntitiesManager::updateEntityPosition(const std::shared_ptr<Entity>& entity, float x, float y) {
    entity->setPosition(x, y);
    for (const std::string& name : entity->listComponentNames()) {
      if (auto* comp = entity->getComponent(name)) {
        if (auto* pos = dynamic_cast<Project::Components::PositionableComponent*>(comp)) {
          pos->setEntityPosition(x, y);
        }
      }
    }
  }

  void EntitiesManager::optimizeEntitiesImpl() {
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
    updateHighCount = updateNormalCount = updateLowCount = updateToRemoveCount = 0;
  }
}
