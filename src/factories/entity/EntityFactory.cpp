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
    
    EntityID entityID = static_cast<EntityID>(lua_tointeger(L, -1));
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

    lua_close(L);

    return true;
  }
}