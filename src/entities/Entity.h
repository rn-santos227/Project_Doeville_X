#ifndef ENTITY_H
#define ENTITY_H

#include "EntityAttribute.h"
#include "EntityCategory.h"
#include "EntityData.h"

#include <functional>
#include <lua.hpp>
#include <memory>
#include <string>
#include <type_traits>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include "components/BaseComponent.h"
#include "components/ComponentType.h"
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

    const EntityData& getData() const { return data; }
    EntityData& getData() { return data; }

    Entity(const Entity&) = delete;
    Entity& operator = (const Entity&) = delete;

    Entity(Entity&&) noexcept = default;
    Entity& operator = (Entity&&) noexcept = default;

    void initialize();
    void update(float deltaTime);
    void render();

    const EntityCategory& getEntityCategory() const { return entityCategory; }

    const std::string& getEntityID() const { return data.id; }
    void setEntityID(const std::string& _id) { data.id = _id; }

    const std::string& getEntityClass() const { return data.entityClass; }
    void setEntityClass(const std::string& _class) { data.entityClass = _class; }

    const std::string& getGroup() const { return data.group; }
    void setGroup(const std::string& _group) { data.group = _group; }

    const std::string& getEntityName() const { return data.name; }
    void setEntityName(const std::string& _name) { data.name = _name; }
    
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
    std::vector<Project::Components::BaseComponent*> getComponentsByType(Project::Components::ComponentType type) const;
    bool canSee(const Project::Entities::Entity* target) const;
    
    bool isActive() const { return data.active; }
    void setActive(bool _value) { data.active = _value; }

    bool isGlobal() const { return data.global; }
    void setGlobal(bool _value) { data.global = _value; }

    void setPosition(float _newX, float _newY) { data.x = _newX; data.y = _newY; }
    void setPosition(float _newX, float _newY, float _newZ) { data.x = _newX; data.y = _newY; data.z = _newZ; }

    float getX() const { return data.x; }
    float getY() const { return data.y; }
    float getZ() const { return data.z; }

  private:
    Project::Components::BoundingBoxComponent* bbox = nullptr;
    Project::Components::GraphicsComponent* gfx = nullptr;
    Project::Components::PhysicsComponent* physics = nullptr;
    Project::Factories::ComponentsFactory& componentsFactory;
    
    EntityCategory entityCategory;
    EntityData data;

    std::unordered_map<std::string, ComponentPtr> components;
    std::unordered_set<EntityAttribute> attributes;
    std::vector<std::string> componentOrder;
        
    Project::Entities::EntitiesManager* entitiesManager = nullptr;
  };
}

#endif
