#include "Entity.h"
#include "EntityAttributeResolver.h"

#include <algorithm>

#include "components/PositionableComponent.h"
#include "components/behavior_component/BehaviorComponent.h"
#include "components/bounding_box_component/BoundingBoxComponent.h"
#include "components/button_component/ButtonComponent.h"
#include "components/camera_component/CameraComponent.h"
#include "components/graphics_component/GraphicsComponent.h"
#include "components/light_component/LightComponent.h"
#include "components/keys_component/KeysComponent.h"
#include "components/meter_component/MeterComponent.h"
#include "components/motion_component/MotionComponent.h"
#include "components/numeric_component/NumericComponent.h"
#include "components/physics_component/PhysicsComponent.h"
#include "components/portal_component/PortalComponent.h"
#include "components/spawner_component/SpawnerComponent.h"
#include "components/timer_component/TimerComponent.h"
#include "components/transform_component/TransformComponent.h"
#include "components/vision_component/VisionComponent.h"
#include "entities/EntitiesManager.h"
#include "factories/component/ComponentsFactory.h"
#include "interfaces/style_interface/Stylable.h"
#include "libraries/constants/Constants.h"
#include "libraries/keys/Keys.h"

namespace Project::Entities {
  using Project::Utilities::LogsManager;
  using Project::Factories::ComponentsFactory;
  using Project::Components::BaseComponent;
  
  namespace Constants = Project::Libraries::Constants;
  namespace Keys = Project::Libraries::Keys;

  Entity::Entity(EntityCategory entityCategory, LogsManager& logsManager, ComponentsFactory& componentsFactory)
  : LuaScriptable(logsManager), componentsFactory(componentsFactory), entityCategory(std::move(entityCategory)) {}

  Entity::~Entity() = default;

  void Entity::initialize() {
    data.x = luaStateWrapper.getGlobalNumber(Keys::X, 0.0f);
    data.y = luaStateWrapper.getGlobalNumber(Keys::Y, 0.0f);
    data.z = luaStateWrapper.getGlobalNumber(Keys::Z, 0.0f);

    luaStateWrapper.callFunctionIfExists(Keys::INITIALIZE);

    auto positionComponent = [this](BaseComponent* comp) {
      if (auto* positionable = dynamic_cast<Components::PositionableComponent*>(comp)) {
        positionable->setEntityPosition(data.x, data.y);
      }
    };

    auto applyComponentStyle = [](Components::BaseComponent* comp) {
      if (auto* stylable = dynamic_cast<Project::Interfaces::Stylable*>(comp)) {
        stylable->applyStyle();
      }
    };


    for (const auto& name : componentOrder) {
      auto it = components.find(name);
      if (it != components.end()) {
        auto& component = it->second;
        if (component) {
          component->onAttach();
          positionComponent(component.get());
          applyComponentStyle(component.get());
        }
      }
    }
  }

  void Entity::update(float deltaTime) {
    luaStateWrapper.setGlobalNumber(Keys::DELTA_TIME, deltaTime);
    luaStateWrapper.callFunctionIfExists(Keys::UPDATE);

    for (const auto& name : componentOrder) {
      auto it = components.find(name);
      if (it != components.end()) {
        auto& component = it->second;
        if (component && component->isActive()) {
          if (dynamic_cast<Components::MotionComponent*>(component.get()) ||
              dynamic_cast<Components::PhysicsComponent*>(component.get()) ||
              dynamic_cast<Components::GraphicsComponent*>(component.get()) ||
              dynamic_cast<Components::BehaviorComponent*>(component.get())) {
            continue;
          }
          component->update(deltaTime);
        }
      }
    }
  }

  void Entity::render() {
    luaStateWrapper.callFunctionIfExists(Keys::RENDER);
    for (const auto& name : componentOrder) {
      auto it = components.find(name);
      if (it != components.end()) {
        auto& component = it->second;
        if (component && component->isActive()) {
          if (dynamic_cast<Components::GraphicsComponent*>(component.get())) {
            continue;
          }
          component->render();
        }
      }
    }
  }

  bool Entity::attachLuaScript(const std::string& scriptPath) {
    if (!LuaScriptable::attachLuaScript(scriptPath)) {
      return false;
    }

    data.global = luaStateWrapper.getGlobalBoolean(Keys::GLOBAL, false);
    data.active = luaStateWrapper.getGlobalBoolean(Keys::ACTIVE, true);
    data.group = luaStateWrapper.getGlobalString(Keys::GROUP, Constants::EMPTY_STRING);

    if (luaStateWrapper.isGlobalTable(Keys::ATTRIBUTES)) {
      luaStateWrapper.iterateGlobalTable(Keys::ATTRIBUTES, [this](lua_State* L, int index) {
        if (lua_isstring(L, -1)) {
          std::string attr = lua_tostring(L, -1);
          EntityAttribute resolved = EntityAttributeResolver::resolve(attr);
          addAttribute(resolved);
        }
      });
    }

    if (luaStateWrapper.isGlobalTable(Keys::COMPONENTS)) {
      luaStateWrapper.iterateGlobalTable(Keys::COMPONENTS, [this](lua_State* L, int index) {
        if (lua_istable(L, -1)) {
          std::string componentName = lua_tostring(L, -2);

          lua_pushvalue(L, -1);
          lua_setglobal(L, componentName.c_str());
          
          ComponentPtr component = componentsFactory.create(componentName, luaStateWrapper, componentName);
          
          lua_pushnil(L);
          lua_setglobal(L, componentName.c_str());

          if (component) {
            addComponent(componentName, std::move(component));
          } else {
            logsManager.logError("Failed to create component: " + componentName);
          }
        }
      });
    }

    return true;
  }

  void Entity::addComponent(const std::string& componentName, ComponentPtr component) {
    if (!component) return;

    if (auto* bboxComp = dynamic_cast<Components::BoundingBoxComponent*>(component.get())) {
      bboxComp->setEntityReference(this);
      bbox = bboxComp;
      if (entitiesManager && !physics) {
        entitiesManager->getPhysicsSystem().addStaticCollider(bboxComp);
      }
    }

    if (auto* button = dynamic_cast<Components::ButtonComponent*>(component.get())) {
      button->setEntityReference(this);
    }

    if (auto* camera = dynamic_cast<Components::CameraComponent*>(component.get())) {
      camera->setEntityReference(this);
    }

    if (auto* gfxComp = dynamic_cast<Components::GraphicsComponent*>(component.get())) {
      gfx = gfxComp;
    }
    
    if (auto* keys = dynamic_cast<Components::KeysComponent*>(component.get())) {
      keys->setEntityReference(this);
    }

    if (auto* light = dynamic_cast<Components::LightComponent*>(component.get())) {
      light->setEntityReference(this);
    }

    if (auto* meter = dynamic_cast<Components::MeterComponent*>(component.get())) {
      meter->setEntityReference(this);
    }

    if (auto* motion = dynamic_cast<Components::MotionComponent*>(component.get())) {
      motion->setEntityReference(this);
      motion->onAttach();
    }

    if (auto* numeric = dynamic_cast<Components::NumericComponent*>(component.get())) {
      numeric->setEntityReference(this);
    }

    if (auto* physicsComp = dynamic_cast<Components::PhysicsComponent*>(component.get())) {
      physicsComp->setEntityReference(this);
      physics = physicsComp;
    }

    if (auto* portal = dynamic_cast<Components::PortalComponent*>(component.get())) {
      portal->setEntityReference(this);
    }

    if (auto* spawner = dynamic_cast<Components::SpawnerComponent*>(component.get())) {
      spawner->setEntityReference(this);
    }

    if (auto* timer = dynamic_cast<Components::TimerComponent*>(component.get())) {
      timer->setEntityReference(this);
    }

    if (auto* transform = dynamic_cast<Components::TransformComponent*>(component.get())) {
      transform->setEntityReference(this);
    }

    if (auto* vision = dynamic_cast<Components::VisionComponent*>(component.get())) {
      vision->setEntityReference(this);
    }

    if (component->getClass().empty()) {
      if (!data.entityClass.empty()) {
        component->setClass(data.entityClass);
      } else {
        component->setClass(data.name);
      }
    }

    bool isNew = components.find(componentName) == components.end();
    components[componentName] = std::move(component);
    if (isNew) {
      componentOrder.push_back(componentName);
    }
    if (entitiesManager) {
      auto* compPtr = components[componentName].get();
      if (auto* motion = dynamic_cast<Components::MotionComponent*>(compPtr)) {
        entitiesManager->getMotionSystem().add(motion);
      } else if (auto* phys = dynamic_cast<Components::PhysicsComponent*>(compPtr)) {
        entitiesManager->getPhysicsSystem().add(phys);
      } else if (auto* gfx = dynamic_cast<Components::GraphicsComponent*>(compPtr)) {
        entitiesManager->getRenderSystem().add(gfx);
      }
    }
  }

  void Entity::removeComponent(const std::string& componentName) {
    auto it = components.find(componentName);
    if (it == components.end()) return;

    auto* ptr = it->second.get();

    if (ptr == bbox) {
      if (entitiesManager && !physics) {
        entitiesManager->getPhysicsSystem().removeStaticCollider(static_cast<Components::BoundingBoxComponent*>(ptr));
      }
      bbox = nullptr;
    }
    if (ptr == gfx) gfx = nullptr;
    if (ptr == physics) physics = nullptr;

    if (dynamic_cast<Components::KeysComponent*>(ptr)) {
      for (auto& [name, comp] : components) {
        if (auto* motion = dynamic_cast<Components::MotionComponent*>(comp.get())) {
          motion->setKeysComponent(nullptr);
        }
      }
    }

    if (entitiesManager) {
      if (auto* motion = dynamic_cast<Components::MotionComponent*>(ptr)) {
        entitiesManager->getMotionSystem().remove(motion);
      } else if (auto* phys = dynamic_cast<Components::PhysicsComponent*>(ptr)) {
        entitiesManager->getPhysicsSystem().remove(phys);
      } else if (auto* gfx = dynamic_cast<Components::GraphicsComponent*>(ptr)) {
        entitiesManager->getRenderSystem().remove(gfx);
      }
    }

    components.erase(it);
    componentOrder.erase(std::remove(componentOrder.begin(), componentOrder.end(), componentName), componentOrder.end());
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
    return componentOrder;
  }
}
