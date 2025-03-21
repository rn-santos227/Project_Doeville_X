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

#include "components/BaseComponent.h"
#include "utilities/logs_manager/LogsManager.h"

using namespace Project::Components;
using namespace Project::Utilities;

namespace Project::Entities {
  class Entity {
  public:
    explicit Entity(EntityID entityID, LogsManager& logsManager);
    virtual ~Entity();

    const EntityID& getEntityID() const { return entityID; }

    void initialize();
    void update(float deltaTime);
    void render();
    bool attachLuaScript(const std::string& scriptPath);
    bool callLuaFunction(const std::string& functionName);

    void addComponent(const std::string& componentName, std::unique_ptr<BaseComponent> component);
    void removeComponent(const std::string& componentName);

  private:
    EntityID entityID;
    lua_State* luaState;
    LogsManager& logsManager;

    std::string entityName;
    std::unordered_map<std::string, std::unique_ptr<BaseComponent>> components;
    
    void handleLuaError(const std::string& errorMessage);
  };
}

#endif