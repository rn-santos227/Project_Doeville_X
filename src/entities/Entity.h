#ifndef ENTITY_H
#define ENTITY_H

#include "EntityID.h"

#include <string>
#include <unordered_map>
#include <memory>
#include <functional>

extern "C" {
  #include "lua.h"
  #include "lauxlib.h"
  #include "lualib.h"
}

#include "utilities/logs_manager/LogsManager.h"

using namespace Project::Utilities;

namespace Project::Entities {
  class Entity {
  public:
    explicit Entity(EntityID entityID, LogsManager& logsManager);
    virtual ~Entity();

    const EntityID& getEntityID() const { return entityID; }

  private:
    EntityID entityID;
    lua_State* luaState;
    LogsManager& logsManager;

    std::unordered_map<std::string, std::function<void()>> components;
    void handleLuaError(const std::string& errorMessage);
  };
}

#endif