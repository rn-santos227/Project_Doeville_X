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
}