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
        component->initialize();
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

  void Entity::handleLuaError(const std::string& errorMessage) {
    logsManager.logError("Entity Lua Error: " + errorMessage);
  }
}