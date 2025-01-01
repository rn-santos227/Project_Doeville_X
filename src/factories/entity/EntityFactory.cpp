#include "EntityFactory.h"

namespace Project::Factories {
  EntityFactory::EntityFactory(EntitiesManager& entitiesManager, LogsManager& logsManager)
  : entitiesManager(entitiesManager), logsManager(logsManager) {}

  EntityFactory::~EntityFactory() {
    entitiesManager.reset();
  }

  bool EntityFactory::createEntityFromLua(const std::string& scriptPath) {
    lua_State* L = luaL_newstate();
    luaL_openlibs(L);

    if (luaL_dofile(L, scriptPath.c_str()) != LUA_OK) {
      logsManager.logError("Failed to load Lua script: " + scriptPath + "\nError: " + lua_tostring(L, -1));
      lua_close(L);
      return false;
    }

    lua_getglobal(L, "entityID");

    if (!lua_isinteger(L, -1)) {
      logsManager.logError("Lua script is missing a valid 'entityID': " + scriptPath);
      lua_close(L);
      return false;
    }

    EntityID entityID = static_cast<EntityID>(lua_tointeger(L, -1));
    lua_pop(L, 1);

    lua_getglobal(L, "entityName");

    if (!lua_isstring(L, -1)) {
      logsManager.logError("Lua script is missing a valid 'entityName': " + scriptPath);
      lua_close(L);
      return false;
    }
    
    std::string entityName = lua_tostring(L, -1);
    lua_pop(L, 1);

    auto entity = std::make_unique<Entity>(entityID, logsManager);

    if (!entity->attachLuaScript(scriptPath)) {
      logsManager.logError("Failed to load Entity from Lua script: " + scriptPath);
      return false;
    }

    if (!entity->callLuaFunction("initialize")) {
      logsManager.logError("Failed to initialize Entity from Lua script: " + scriptPath);
      return false;
    }

    entitiesManager.addEntity(entityName, std::move(entity));

    lua_close(L);
    return true;
  }
}