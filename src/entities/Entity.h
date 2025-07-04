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

namespace Project::Entities {
  class Entity : public Project::Utilities::LuaScriptable {
  public:
    explicit Entity(EntityCategory entityCategory, 
      Project::Utilities::LogsManager& logsManager, 
      Project::Factories::ComponentsFactory& componentsFactory);
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

    void addComponent(const std::string& componentName, std::unique_ptr<Project::Components::BaseComponent> component);
    void removeComponent(const std::string& componentName);
    bool hasComponent(const std::string& componentName) const;

    Project::Components::BaseComponent* getComponent(const std::string& componentName);
    std::vector<std::string> listComponentNames() const;

    void setPosition(float newX, float newY) { x = newX; y = newY; }
    void setPosition(float newX, float newY, float newZ) { x = newX; y = newY; z = newZ; }

    float getX() const { return x; }
    float getY() const { return y; }
    float getZ() const { return z; }

  private:
    Project::Factories::ComponentsFactory& componentsFactory;
    EntityCategory entityCategory;

    std::unordered_map<std::string, std::unique_ptr<Project::Components::BaseComponent>> components;
    std::string entityName;

    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
  };
}

#endif
