#include "Entity.h"

namespace Project::Entities {
  Entity::Entity(EntityID entityID, LogsManager& logsManager)
  : entityID(std::move(entityID)), logsManager(logsManager) {
    luaState = luaL_newstate();
  }

  void Entity::handleLuaError(const std::string& errorMessage) {
    logsManager.logError("Entity Lua Error: " + errorMessage);
  }
}