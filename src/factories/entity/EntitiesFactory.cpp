#include "EntitiesFactory.h"

#include <filesystem>

#include "bindings/LuaBindings.h"
#include "libraries/constants/ScriptConstants.h"
#include "libraries/keys/Keys.h"
#include "states/GameStateManager.h"
#include "utilities/lua_state_wrapper/LuaStateWrapper.h"

namespace Project::Factories {
  using Project::Utilities::LogsManager;
  using Project::Utilities::ConfigReader;
  using Project::Entities::Entity;
  using Project::Entities::EntityCategory;
  using Project::Factories::EntitiesFactory;
  using Project::States::GameStateManager;

  namespace Constants = Project::Libraries::Constants;
  namespace Keys = Project::Libraries::Keys;

  EntitiesFactory::EntitiesFactory(
    LogsManager& logsManager, ConfigReader& configReader,
    ComponentsFactory& componentsFactory, GameStateManager& gameStateManager)
    : logsManager(logsManager), configReader(configReader),
      componentsFactory(componentsFactory), gameStateManager(gameStateManager) {
        int limit = configReader.getIntValue(Keys::POOLS_SECTION, Keys::POOL_ENTITY_MAX, Constants::DEFAULT_COMPONENT_SIZE);
        Project::Helpers::EntityPool::getInstance().setMaxSize(static_cast<size_t>(limit));
      }

  EntitiesFactory::~EntitiesFactory() {
    entityTemplates.clear();
  }

  EntitiesFactory::EntityPtr EntitiesFactory::createEntityFromLua(const std::string& scriptPath) {
    EntityPtr entity = loadEntityTemplateFromLua(scriptPath);
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

  EntitiesFactory::EntityPtr EntitiesFactory::cloneEntity(const std::string& entityName) {
    auto it = entityTemplates.find(entityName);
    if (logsManager.checkAndLogError(it == entityTemplates.end(), "Entity template not found: " + entityName)) {
      return nullptr;
    }

    EntityCategory category = it->second->getEntityCategory();
    EntityPtr clone = Project::Helpers::EntityPool::getInstance().make_unique(category, logsManager, componentsFactory);
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
        clone->getLuaStateWrapper().registerFunction(
          Project::Libraries::Keys::LUA_CHANGE_STATE,
          Project::Bindings::LuaBindings::lua_factoryChangeState, 
          &gameStateManager
        );
      }
    }

    return clone;
  }

  EntitiesFactory::EntityPtr EntitiesFactory::loadEntityTemplateFromLua(const std::string& scriptPath) {
    Project::Utilities::LuaStateWrapper lua(logsManager);
    if (!lua.loadScript(scriptPath)) {
      logsManager.logError("Failed to load entity script: " + scriptPath);
      return nullptr;
    }

    std::string name;
    lua_State* L = lua.get();
    if (L && lua_gettop(L) > 0 && lua_istable(L, -1)) {
      lua_getfield(L, -1, Project::Libraries::Keys::NAME);
      if (lua_isstring(L, -1)) {
        size_t len = 0;
        const char* cstr = lua_tolstring(L, -1, &len);
        if (cstr && len > 0) {
          name.assign(cstr, len);
        }
      }
      lua_pop(L, 2);
    }

    if (name.empty() && L) {
      lua_getglobal(L, Project::Libraries::Keys::NAME);
      if (lua_isstring(L, -1)) {
        size_t len = 0;
        const char* cstr = lua_tolstring(L, -1, &len);
        if (cstr && len > 0) {
          name.assign(cstr, len);
        }
      }
      lua_pop(L, 1);
    }

    if (name.empty()) {
      std::filesystem::path p(scriptPath);
      std::string filename = p.filename().string();
      std::string suffix = Project::Libraries::Constants::LUA_ENTITY_SUFFIX;
      if (filename.size() >= suffix.size() && filename.compare(filename.size() - suffix.size(), suffix.size(), suffix) == 0) {
        name = filename.substr(0, filename.size() - suffix.size());
      } else {
        name = p.stem().string();
      }
    }

    EntityCategory category = EntityCategory::ENVIRONMENT;

    EntityPtr entity = Project::Helpers::EntityPool::getInstance().make_unique(
      category, logsManager, componentsFactory
    );

    if (logsManager.checkAndLogError(!entity->attachLuaScript(scriptPath), "Failed to attach Lua script: " + scriptPath)) {
      return nullptr;
    }

    entity->setEntityName(name);
    entityScriptPaths[name] = scriptPath;
    return entity;
  }
}
