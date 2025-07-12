#include "EntitiesFactory.h"

#include <filesystem>

#include "states/GameStateManager.h"

namespace Project::Factories {
  using Project::Utilities::LogsManager;
  using Project::Entities::Entity;
  using Project::Entities::EntityCategory;
  using Project::Factories::EntitiesFactory;
  using Project::States::GameStateManager;

  EntitiesFactory::EntitiesFactory(LogsManager& logsManager, ComponentsFactory& componentsFactory, GameStateManager& gameStateManager)
    : logsManager(logsManager), componentsFactory(componentsFactory), gameStateManager(gameStateManager) {}

  EntitiesFactory::~EntitiesFactory() {
    entityTemplates.clear();
  }

  int EntitiesFactory::lua_changeState(lua_State* L) {
    auto* manager = static_cast<States::GameStateManager*>(lua_touserdata(L, lua_upvalueindex(1)));
    if (!manager) {
      return luaL_error(L, "Invalid GameStateManager reference in lua_changeState.");
    }

    const char* name = luaL_checkstring(L, 1);
    if (!name) {
      luaL_error(L, "Expected a state name string.");
      return 0;
    }

    manager->changeState(name);
    return 0;
  }

  std::unique_ptr<Entity> EntitiesFactory::createEntityFromLua(const std::string& scriptPath) {
    std::unique_ptr<Entity> entity = loadEntityTemplateFromLua(scriptPath);
    if (logsManager.checkAndLogError(!entity, "Failed to load entity template from Lua: " + scriptPath)) {
      return nullptr;
    }

    std::string name = entity->getEntityName();
    if (logsManager.checkAndLogError(name.empty(), "Entity name is empty after loading from Lua: " + scriptPath)) {
      return nullptr;
    }

    entityTemplates[name] = std::move(entity);
    entityScriptPaths[name] = scriptPath;
    return cloneEntity(name);
  }

  bool EntitiesFactory::hasEntityTemplate(const std::string& entityName) const {
    return entityTemplates.find(entityName) != entityTemplates.end();
  }

  bool EntitiesFactory::isEntityGlobal(const std::string& entityName) const {
    auto it = entityTemplates.find(entityName);
    if (it != entityTemplates.end()) {
      return it->second->isGlobal();
    }
    return false;
  }  

  std::unique_ptr<Entity> EntitiesFactory::cloneEntity(const std::string& entityName) {
    auto it = entityTemplates.find(entityName);
    if (logsManager.checkAndLogError(it == entityTemplates.end(), "Entity template not found: " + entityName)) {
      return nullptr;
    }

    EntityCategory category = it->second->getEntityCategory();
    std::unique_ptr<Entity> clone = std::make_unique<Entity>(category, logsManager, componentsFactory);
    clone->setEntityName(entityName);

    lua_State* templateState = it->second->getLuaState();
    if (templateState) {
      auto pathIt = entityScriptPaths.find(entityName);
      std::string scriptPath = (pathIt != entityScriptPaths.end()) ? pathIt->second : "";
      if (!scriptPath.empty()) {
        clone->attachLuaScript(scriptPath);
      } else {
        logsManager.logWarning("Script path for entity '" + entityName + "' not found. Using default path.");
        clone->attachLuaScript("scripts/entities/" + entityName + ".entity.lua");
      }

      lua_State* L = clone->getLuaState();
      if (L) {
        clone->getLuaStateWrapper().registerFunction("changeState", lua_changeState, &gameStateManager);
      }
    }

    return clone;
  }

  std::unique_ptr<Entity> EntitiesFactory::loadEntityTemplateFromLua(const std::string& scriptPath) {
    std::filesystem::path p(scriptPath);
    std::string filename = p.filename().string();
    std::string suffix = ".entity.lua";
    std::string name = filename;
    if (filename.size() >= suffix.size() && filename.compare(filename.size() - suffix.size(), suffix.size(), suffix) == 0) {
      name = filename.substr(0, filename.size() - suffix.size());
    } else {
      name = p.stem().string();
    }

    EntityCategory category = EntityCategory::ENVIRONMENT;

    std::unique_ptr<Entity> entity = std::make_unique<Entity>(category, logsManager, componentsFactory);
    entity->setEntityName(name);

    if (logsManager.checkAndLogError(!entity->attachLuaScript(scriptPath), "Failed to attach Lua script: " + scriptPath)) {
      return nullptr;
    }

    entityScriptPaths[name] = scriptPath;
    return entity;
  }
}
