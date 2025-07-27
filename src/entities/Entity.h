#ifndef ENTITY_H
#define ENTITY_H

#include "EntityAttribute.h"
#include "EntityCategory.h"

#include <functional>
#include <lua.hpp>
#include <memory>
#include <string>
#include <type_traits>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include "components/BaseComponent.h"
#include "components/bounding_box_component/BoundingBoxComponent.h"
#include "components/graphics_component/GraphicsComponent.h"
#include "components/physics_component/PhysicsComponent.h"
#include "interfaces/render_interface/Renderable.h"
#include "interfaces/update_interface/Updatable.h"
#include "utilities/lua_scriptable/LuaScriptable.h"

namespace Project::Factories {
  class ComponentsFactory;
}

namespace Project::Entities {
  class EntitiesManager;
  class Entity : public Project::Utilities::LuaScriptable, public Project::Interfaces::Renderable, public Project::Interfaces::Updatable {
  public:
    explicit Entity(EntityCategory entityCategory, 
      Project::Utilities::LogsManager& logsManager, 
      Project::Factories::ComponentsFactory& componentsFactory);
    virtual ~Entity();

    Entity(const Entity&) = delete;
    Entity& operator = (const Entity&) = delete;

    Entity(Entity&&) noexcept = default;
    Entity& operator = (Entity&&) noexcept = default;

    void initialize();
    void update(float deltaTime);
    void render();

    const EntityCategory& getEntityCategory() const { return entityCategory; }

    const std::string& getEntityID() const { return entityID; }
    void setEntityID(const std::string& _id) { entityID = _id; }
    
    const std::string& getEntityClass() const { return entityClass; }
    void setEntityClass(const std::string& _class) { entityClass = _class; }

    const std::string& getGroup() const { return entityGroup; }
    void setGroup(const std::string& _group) { entityGroup = _group; }

    const std::string& getEntityName() const { return entityName; }
    void setEntityName(const std::string& _name) { entityName = _name; }
    
    void setEntitiesManager(Project::Entities::EntitiesManager* _manager) { entitiesManager = _manager; }
    Project::Entities::EntitiesManager* getEntitiesManager() const { return entitiesManager; }

    void registerLuaFunction(const std::string& _name, lua_CFunction _function) {
      luaStateWrapper.registerFunction(_name, _function);
    }

    void registerLuaFunction(const std::string& _name, lua_CFunction _function, void* _data) {
      luaStateWrapper.registerFunction(_name, _function, _data);
    }
    
    bool attachLuaScript(const std::string& scriptPath);
    
    using ComponentPtr = std::unique_ptr<Project::Components::BaseComponent, std::function<void(Project::Components::BaseComponent*)>>;
    void addComponent(const std::string& componentName, ComponentPtr component);    
    
    void removeComponent(const std::string& componentName);
    bool hasComponent(const std::string& componentName) const;

    void addAttribute(EntityAttribute attribute) { attributes.insert(attribute); }
    void removeAttribute(EntityAttribute attribute) { attributes.erase(attribute); }
    bool hasAttribute(EntityAttribute attribute) const { return attributes.count(attribute) > 0; }
    const std::unordered_set<EntityAttribute>& getAttributes() const { return attributes; }

    Project::Components::BaseComponent* getComponent(const std::string& componentName);
    Project::Components::BoundingBoxComponent* getBoundingBoxComponent() const { return bbox; }
    Project::Components::GraphicsComponent* getGraphicsComponent() const { return gfx; }
    Project::Components::PhysicsComponent* getPhysicsComponent() const { return physics; }
    std::vector<std::string> listComponentNames() const;
    
    bool isActive() const { return active; }
    void setActive(bool _value) { active = _value; }

    bool isGlobal() const { return global; }
    void setGlobal(bool _value) { global = _value; }

    void setPosition(float _newX, float _newY) { x = _newX; y = _newY; }
    void setPosition(float _newX, float _newY, float _newZ) { x = _newX; y = _newY; z = _newZ; }

    float getX() const { return x; }
    float getY() const { return y; }
    float getZ() const { return z; }

  private:
    Project::Components::BoundingBoxComponent* bbox = nullptr;
    Project::Components::GraphicsComponent* gfx = nullptr;
    Project::Components::PhysicsComponent* physics = nullptr;
    Project::Factories::ComponentsFactory& componentsFactory;
    EntityCategory entityCategory;

    std::unordered_map<std::string, ComponentPtr> components;
    std::unordered_set<EntityAttribute> attributes;

    std::string entityID;
    std::string entityClass;
    std::string entityGroup;
    std::string entityName;
    
    Project::Entities::EntitiesManager* entitiesManager = nullptr;

    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;

    bool active = true;
    bool global = false;
  };
}

#endif
