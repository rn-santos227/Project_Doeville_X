#ifndef ENTITY_H
#define ENTITY_H

#include "EntityCategory.h"

#include <functional>
#include <memory>
#include <string>
#include <type_traits>
#include <vector>
#include <unordered_map>

#include "components/BaseComponent.h"
#include "factories/component/ComponentsFactory.h"

#include "utilities/lua_scriptable/LuaScriptable.h"

using namespace Project::Components;
using namespace Project::Factories;
using namespace Project::Utilities;

namespace Project::Entities {
  class Entity : public LuaScriptable {
  public:
    explicit Entity(EntityCategory entityCategory, LogsManager& logsManager, ComponentsFactory& componentsFactory);
    virtual ~Entity();

    Entity(const Entity&) = delete;
    Entity& operator = (const Entity&) = delete;

    Entity(Entity&&) noexcept = default;
    Entity& operator = (Entity&&) noexcept = default;

    const EntityCategory& getEntityCategory() const { return entityCategory; }
    const std::string& getEntityName() const { return entityName; }
    void setEntityName(const std::string& name) { entityName = name; }

    void initialize();
    void update(float deltaTime);
    void render();
    
    bool attachLuaScript(const std::string& scriptPath);
    bool callLuaFunction(const std::string& functionName);

    void addComponent(const std::string& componentName, std::unique_ptr<BaseComponent> component);
    void removeComponent(const std::string& componentName);
    bool hasComponent(const std::string& componentName) const;

    BaseComponent* getComponent(const std::string& componentName);
    std::vector<std::string> listComponentNames() const;

    lua_State* getLuaState() const { return luaStateWrapper.get(); }

    void setPosition(float newX, float newY) { x = newX; y = newY; }
    void setPosition(float newX, float newY, float newZ) { x = newX; y = newY; z = newZ; }

    float getX() const { return x; }
    float getY() const { return y; }
    float getZ() const { return z; }

  private:
    LogsManager& logsManager;
    ComponentsFactory& componentsFactory;
  
    LuaStateWrapper luaStateWrapper;
    EntityCategory entityCategory;

    std::unordered_map<std::string, std::unique_ptr<BaseComponent>> components;
    std::string entityName;

    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
  };
}

#endif
