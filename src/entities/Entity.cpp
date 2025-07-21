#include "Entity.h"

#include "components/PositionableComponent.h"
#include "components/camera_component/CameraComponent.h"
#include "components/keys_component/KeysComponent.h"
#include "components/motion_component/MotionComponent.h"
#include "components/physics_component/PhysicsComponent.h"
#include "components/spawner_component/SpawnerComponent.h"
#include "components/timer_component/TimerComponent.h"
#include "components/transform_component/TransformComponent.h"
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
  : LuaScriptable(logsManager), componentsFactory(componentsFactory), entityCategory(std::move(entityCategory)),
    x(0.0f), y(0.0f), z(0.0f) {}

  Entity::~Entity() = default;

  void Entity::initialize() {
    x = luaStateWrapper.getGlobalNumber(Keys::X, 0.0f);
    y = luaStateWrapper.getGlobalNumber(Keys::Y, 0.0f);
    z = luaStateWrapper.getGlobalNumber(Keys::Z, 0.0f);

    luaStateWrapper.callFunctionIfExists(Keys::INITIALIZE);

    auto positionComponent = [this](BaseComponent* comp) {
      if (auto* positionable = dynamic_cast<Components::PositionableComponent*>(comp)) {
        positionable->setEntityPosition(static_cast<int>(x), static_cast<int>(y));
      }
    };

    auto applyComponentStyle = [](Components::BaseComponent* comp) {
      if (auto* stylable = dynamic_cast<Project::Interfaces::Stylable*>(comp)) {
        stylable->applyStyle();
      }
    };


    for (auto& [name, component] : components) {
      if (component) {
        component->onAttach();
        positionComponent(component.get());
        applyComponentStyle(component.get());
      }
    }
  }

  void Entity::update(float deltaTime) {
    luaStateWrapper.setGlobalNumber(Keys::DELTA_TIME, deltaTime);
    luaStateWrapper.callFunctionIfExists(Keys::UPDATE);

    for (auto& [name, component] : components) {
      if (component && component->isActive()) {
        component->update(deltaTime);
      }
    }
  }

  void Entity::render() {
    luaStateWrapper.callFunctionIfExists(Keys::RENDER);
    for (auto& [name, component] : components) {
      if (component && component->isActive()) {
        component->render();
      }
    }
  }

  bool Entity::attachLuaScript(const std::string& scriptPath) {
    if (!LuaScriptable::attachLuaScript(scriptPath)) {
      return false;
    }

    global = luaStateWrapper.getGlobalBoolean(Keys::GLOBAL, false);
    entityGroup = luaStateWrapper.getGlobalString(Keys::GROUP, Constants::EMPTY_STRING);

    if (luaStateWrapper.isGlobalTable(Keys::COMPONENTS)) {
      luaStateWrapper.iterateGlobalTable(Keys::COMPONENTS, [this](lua_State* L, int index) {
        if (lua_istable(L, -1)) {
          std::string componentName = lua_tostring(L, -2);

          lua_pushvalue(L, -1);
          lua_setglobal(L, componentName.c_str());
          std::unique_ptr<BaseComponent> component = componentsFactory.create(componentName, luaStateWrapper, componentName);
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

  void Entity::addComponent(const std::string& componentName, std::unique_ptr<BaseComponent> component) {
    if (!component) return;

    if (auto* camera = dynamic_cast<Components::CameraComponent*>(component.get())) {
      camera->setEntityReference(this);
    }
    
    if (auto* keys = dynamic_cast<Components::KeysComponent*>(component.get())) {
      keys->setEntityReference(this);
    }

    if (auto* motion = dynamic_cast<Components::MotionComponent*>(component.get())) {
      motion->setEntityReference(this);
    }

    if (auto* physics = dynamic_cast<Components::PhysicsComponent*>(component.get())) {
      physics->setEntityReference(this);
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

    if (component->getClass().empty()) {
      if (!entityClass.empty()) {
        component->setClass(entityClass);
      } else {
        component->setClass(entityName);
      }
    }

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
