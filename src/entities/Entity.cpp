#include "Entity.h"

namespace Project::Entities {
  Entity::Entity(EntityCategory entityCategory, LogsManager& logsManager)
  : entityCategory(std::move(entityCategory)), logsManager(logsManager) {
    luaState = luaL_newstate();
    if (luaState) {
      luaL_openlibs(luaState);
    } else {
      logsManager.logError("Failed to create Lua state for Entity: " + entityName);
    }
  }

  Entity::~Entity() {
    if (luaState) {
      lua_close(luaState);
      luaState = nullptr;
    }
  }

  void Entity::initialize() {
    callLuaFunction("initialize");
    for (auto& [name, component] : components) {
      if (component) {
        component->onAttach();
      }
    }
  }

  void Entity::update(float deltaTime) {
    lua_pushnumber(luaState, deltaTime);
    lua_setglobal(luaState, "deltaTime");

    callLuaFunction("update");
    for (auto& [name, component] : components) {
      if (component) {
        component->update(deltaTime);
      }
    }
  }

  void Entity::render() {
    callLuaFunction("render");
    for (auto& [name, component] : components) {
      if (component) {
        component->render();
      }
    }
  }

  bool Entity::attachLuaScript(const std::string& scriptPath) {
    if (logsManager.checkAndLogError(!luaState, "Lua state is null for Entity: " + entityName)) {
      logsManager.flushLogs();
      return false;
    }

    if (luaL_dofile(luaState, scriptPath.c_str()) != LUA_OK) {
      std::string luaError = lua_tostring(luaState, -1);
      handleLuaError("Failed to load Lua script: " + luaError);
      lua_pop(luaState, 1);
      return false;
    }

    return true;
  }

  void Entity::handleLuaError(const std::string& errorMessage) {
    logsManager.logError("Entity Lua Error: " + errorMessage);
  }
}