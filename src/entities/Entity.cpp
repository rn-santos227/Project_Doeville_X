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
    for (auto& [name, component] : components) {
      if (component) {
        component->initialize();
      }
    }

    callLuaFunction("initialize");
  }

  void Entity::update(float deltaTime) {
    for (auto& [name, component] : components) {
      if (component) {
        component->update(deltaTime);
      }
    }

    lua_pushnumber(luaState, deltaTime);
    lua_setglobal(luaState, "deltaTime");
  }

  void Entity::handleLuaError(const std::string& errorMessage) {
    logsManager.logError("Entity Lua Error: " + errorMessage);
  }
}