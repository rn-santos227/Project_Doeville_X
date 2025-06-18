#include "Entity.h"

namespace Project::Entities {
  Entity::Entity(EntityCategory entityCategory, LogsManager& logsManager, ComponentsFactory& componentsFactory)
  : LuaScriptable(logsManager), componentsFactory(componentsFactory), entityCategory(std::move(entityCategory)),  
    x(0.0f), y(0.0f), z(0.0f) {}

  Entity::~Entity() = default;

  void Entity::initialize() {
    x = luaStateWrapper.getGlobalNumber("x", 0.0f);
    y = luaStateWrapper.getGlobalNumber("y", 0.0f);
    z = luaStateWrapper.getGlobalNumber("z", 0.0f);

    luaStateWrapper.callFunctionIfExists("initialize");
    for (auto& [name, component] : components) {
      if (component) {
        component->onAttach();
      }
    }
  }

  void Entity::update(float deltaTime) {
    luaStateWrapper.setGlobalNumber("deltaTime", deltaTime);
    luaStateWrapper.callFunctionIfExists("update");

    for (auto& [name, component] : components) {
      if (component) {
        component->update(deltaTime);
      }
    }
  }

  void Entity::render() {
    luaStateWrapper.callFunctionIfExists("render");
    for (auto& [name, component] : components) {
      if (component) {
        component->render();
      }
    }
  }

  bool Entity::attachLuaScript(const std::string& scriptPath) {
    if (logsManager.checkAndLogError(!luaStateWrapper.isValid(), "Lua state is null for Entity: " + entityName)) {
      return false;
    }

    if (logsManager.checkAndLogError(!luaStateWrapper.loadScript(scriptPath), "Failed to load Lua script: " + scriptPath)) {
      return false;
    }

    if (luaStateWrapper.isGlobalTable("components")) {
      luaStateWrapper.iterateGlobalTable("components", [this](lua_State* L, int index) {
        if (lua_istable(L, -1)) {
          std::string componentName = lua_tostring(L, -2);
          std::unique_ptr<BaseComponent> component = componentsFactory.create(componentName, luaStateWrapper);
          if (component) {
            addComponent(componentName, std::move(component));
          } else {
            logsManager.logError("Failed to create component: " + componentName);
          }
        }
        lua_pop(L, 1);
      });
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
