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
        handleLuaError("Error calling Lua function 'update'");
      }
    } else {
      lua_pop(luaState, 1);
    }
  }

  void Entity::render() {
    for (const auto& [name, component] : components) {
      component->render();
    }

    lua_getglobal(luaState, "render");
    if (lua_isfunction(luaState, -1)) {
      if (lua_pcall(luaState, 0, 0, 0) != LUA_OK) {
        handleLuaError("Error calling Lua function 'render'");
      }
    } else {
      lua_pop(luaState, 1);
    }
  }

  void Entity::addComponent(const std::string& componentName, std::unique_ptr<Components::BaseComponent> component) {
    components[componentName] = std::move(component);
    logsManager.logMessage("Component added: " + componentName + " to Entity ID: " + std::to_string(static_cast<int>(entityID)));
  }

  void Entity::removeComponent(const std::string& componentName) {
    components.erase(componentName);
    logsManager.logMessage("Component removed: " + componentName + " from Entity ID: " + std::to_string(static_cast<int>(entityID)));
  }

  bool Entity::attachLuaScript(const std::string& scriptPath) {
    if (luaL_dofile(luaState, scriptPath.c_str()) != LUA_OK) {
      handleLuaError("Failed to load Lua script: " + scriptPath);
      return false;
    }

    logsManager.logMessage("Lua script attached: " + scriptPath);
    return true;
  }

  bool Entity::callLuaFunction(const std::string& functionName) {
    lua_getglobal(luaState, functionName.c_str());

    if (!lua_isfunction(luaState, -1)) {
      handleLuaError("Lua function not found: " + functionName);
      lua_pop(luaState, 1);
      return false;
    }

    if (lua_pcall(luaState, 0, 0, 0) != LUA_OK) {
      handleLuaError("Error calling Lua function '" + functionName + "'");
      lua_pop(luaState, 1);
      return false;
    }

    logsManager.logMessage("Lua function called successfully: " + functionName);
  }

  void Entity::handleLuaError(const std::string& errorMessage) {
    logsManager.logError(errorMessage);

    if (lua_gettop(luaState) > 0 && lua_isstring(luaState, -1)) {
       logsManager.logError("Lua error details: " + std::string(lua_tostring(luaState, -1)));
       lua_pop(luaState, 1);
    }
  }
}