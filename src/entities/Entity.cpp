#include "Entity.h"

namespace Project::Entities {
  Entity::Entity(EntityCategory entityCategory, LogsManager& logsManager, ComponentsFactory& componentsFactory)
  : entityCategory(std::move(entityCategory)), 
  logsManager(logsManager), componentsFactory(componentsFactory), 
  luaStateWrapper(logsManager), x(0.0f), y(0.0f), z(0.0f) {
    if (logsManager.checkAndLogError(!luaStateWrapper.isValid(), "Failed to create Lua state for Entity: " + entityName)) {
      logsManager.flushLogs();
      return;
    }

    luaL_openlibs(luaStateWrapper.get());
  }

  Entity::~Entity() = default;

  void Entity::initialize() {
    lua_State* L = luaStateWrapper.get();
    lua_getglobal(L, "x");
    if (lua_isnumber(L, -1)) x = static_cast<float>(lua_tonumber(L, -1));
    lua_pop(L, 1);

    lua_getglobal(L, "y");
    if (lua_isnumber(L, -1)) y = static_cast<float>(lua_tonumber(L, -1));
    lua_pop(L, 1);

    lua_getglobal(L, "z");
    if (lua_isnumber(L, -1)) z = static_cast<float>(lua_tonumber(L, -1));
    lua_pop(L, 1);

    callLuaFunction("initialize");
    for (auto& [name, component] : components) {
      if (component) {
        component->onAttach();
      }
    }
  }

  void Entity::update(float deltaTime) {
    lua_State* L = luaStateWrapper.get();
    lua_pushnumber(L, deltaTime);
    lua_setglobal(L, "deltaTime");

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
    if (logsManager.checkAndLogError(!luaStateWrapper.isValid(), "Lua state is null for Entity: " + entityName)) {
      logsManager.flushLogs();
      return false;
    }

    if (logsManager.checkAndLogError(!luaStateWrapper.loadScript(scriptPath), "Failed to load Lua script: " + scriptPath)) {
      logsManager.flushLogs();
      return false;
    }

    lua_State* L = luaStateWrapper.get();
    lua_getglobal(L, "components");
    if (lua_istable(L, -1)) {
      lua_pushnil(L);
      while (lua_next(L, -2)) {
        std::string componentName = lua_tostring(L, -2);
        if (lua_istable(L, -1)) {
          std::unique_ptr<BaseComponent> component = componentsFactory.create(componentName, L);
          if (component) {
            addComponent(componentName, std::move(component));
          } else {
            logsManager.logError("Failed to create component: " + componentName);
          }
        }
      }
    }

    return true;
  }

  bool Entity::callLuaFunction(const std::string& functionName) {
    if (logsManager.checkAndLogError(!luaStateWrapper.isValid(), "Lua state is null for Entity: " + entityName)) {
      logsManager.flushLogs();
      return false;
    }

    lua_State* L = luaStateWrapper.get();
    lua_getglobal(L, functionName.c_str());
    if (!lua_isfunction(L, -1)) {
      lua_pop(L, 1);
      return false;
    }

    if (lua_pcall(L, 0, 0, 0) != LUA_OK) {
      std::string error = lua_tostring(L, -1);
      luaStateWrapper.handleLuaError("Lua function call failed: " + error);
      lua_pop(L, 1);
      return false;
    }

    return true;
  }

  void Entity::addComponent(const std::string& componentName, std::unique_ptr<BaseComponent> component) {
    if (!component) return;
    components[componentName] = std::move(component);
  }

  void Entity::removeComponent(const std::string& componentName) {
    components.erase(componentName);
  }

  bool Entity::hasComponent(const std::string& componentName) const {
    return components.find(componentName) != components.end();
  }

  BaseComponent* Entity::getComponent(const std::string& componentName) {
    auto it = components.find(componentName);
    if (it != components.end() && it->second) {
      return it->second.get();
    }
    return nullptr;
  }

  std::vector<std::string> Entity::listComponentNames() const {
    std::vector<std::string> names;
    for (const auto& [name, _] : components) {
      names.push_back(name);
    }
    return names;
  }
}