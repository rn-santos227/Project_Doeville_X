#include "Entity.h"

namespace Project::Entities {
  Entity::Entity(EntityID entityID, LogsManager& logsManager)
  : entityID(std::move(entityID)), logsManager(logsManager) {
    luaState = luaL_newstate();
    if (luaState) {
      luaL_openlibs(luaState);
    } else {
      logsManager.logError("Failed to create Lua state for Entity: " + std::to_string(entityID));
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

      }
    }
  }

  void Entity::handleLuaError(const std::string& errorMessage) {
    logsManager.logError("Entity Lua Error: " + errorMessage);
  }
}