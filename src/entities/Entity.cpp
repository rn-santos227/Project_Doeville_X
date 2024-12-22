#include "Entity.h"

namespace Project::Entities {
  Entity::Entity(EntityID id, LogsManager& logsManager)
  : entityID(id), logsManager(logsManager), luaState(luaL_newstate()) {
    luaL_openlibs(luaState);
    logsManager.logMessage("Entity created with ID: " + std::to_string(static_cast<int>(entityID)));
  }
  
  Entity::~Entity() {
    if (luaState) {
      lua_close(luaState);
      logsManager.logMessage("Lua state closed for Entity ID: " + std::to_string(static_cast<int>(entityID)));
    }
  }

  void Entity::update(float deltaTime) {
    for (const auto& [name, component] : components) {
      component->update(deltaTime);
    }

    lua_getglobal(luaState, "update");
    if (lua_isfunction(luaState, -1)) {
      lua_pushnumber(luaState, deltaTime);
      if (lua_pcall(luaState, 1, 0, 0) != LUA_OK) {

      }
    } else {
      lua_pop(luaState, 1);
    }
  }

  void Entity::handleLuaError(const std::string& errorMessage) {
    logsManager.logError(errorMessage);

    if (lua_gettop(luaState) > 0 && lua_isstring(luaState, -1)) {
       logsManager.logError("Lua error details: " + std::string(lua_tostring(luaState, -1)));
       lua_pop(luaState, 1);
    }
  }
}