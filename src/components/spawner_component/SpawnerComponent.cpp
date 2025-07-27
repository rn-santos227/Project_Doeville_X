#include "SpawnerComponent.h"

#include "bindings/LuaBindings.h"
#include "components/motion_component/MotionComponent.h"
#include "components/physics_component/PhysicsComponent.h"
#include "entities/Entity.h"
#include "entities/EntitiesManager.h"
#include "factories/entity/EntitiesFactory.h"
#include "states/GameState.h"
#include "libraries/categories/ComponentCategories.h"
#include "libraries/keys/Keys.h"

namespace Project::Components {
  using Project::Entities::Entity;
  using Project::Entities::EntitiesManager;
  using Project::Factories::EntitiesFactory;
  using Project::States::GameState;

  namespace Components = Project::Libraries::Categories::Components;
  namespace Keys = Project::Libraries::Keys;

  SpawnerComponent::SpawnerComponent(Project::Utilities::LogsManager& logsManager)
    : BaseComponent(logsManager) {}

  void SpawnerComponent::update(float deltaTime) {
    if (!isActive()) return;
    if (cooldown > 0.0f) {
      cooldown -= deltaTime;
      if (cooldown < 0.0f) cooldown = 0.0f;
    }
  }

  void SpawnerComponent::build(Project::Utilities::LuaStateWrapper& luaStateWrapper, const std::string& tableName) {
    templateName = luaStateWrapper.getTableString(tableName, Keys::TEMPLATE, Constants::EMPTY_STRING);
    rate = static_cast<float>(luaStateWrapper.getTableNumber(tableName, Keys::RATE, rate));
  }

  void SpawnerComponent::spawn(float _offsetX, float _offsetY, float _velocityX, float _velocityY) {
    if (!owner || cooldown > 0.0f) return;
    
    EntitiesManager* manager = owner->getEntitiesManager();
    if (!manager) return;
    
    GameState* state = manager->getGameState();
    if (!state) return;
    
    EntitiesFactory* factory = state->getEntitiesFactory();
    if (!factory) return;

    if (templateName.empty()) return;

    EntitiesFactory::EntityPtr ent = factory->cloneEntity(templateName);
    if (!ent) return;

    float x = owner->getX() + _offsetX;
    float y = owner->getY() + _offsetY;
    ent->getLuaStateWrapper().setGlobalNumber(Keys::X, x);
    ent->getLuaStateWrapper().setGlobalNumber(Keys::Y, y);
    ent->initialize();

    if (auto* physics = dynamic_cast<PhysicsComponent*>(ent->getComponent(Components::PHYSICS_COMPONENT))) {
      physics->setVelocity(_velocityX, _velocityY);
    } else if (auto* motion = dynamic_cast<MotionComponent*>(ent->getComponent(Components::MOTION_COMPONENT))) {
      motion->setRawVelocity(_velocityX, _velocityY);
    }

    std::shared_ptr<Entity> shared = std::move(ent);
    manager->addEntity(shared);
    cooldown = rate;
  }

  void SpawnerComponent::setEntityReference(Entity* entity) {
    owner = entity;
    if (owner) {
      owner->registerLuaFunction(Keys::LUA_FUNC_SPAWN, Project::Bindings::LuaBindings::lua_spawn, this);
    }
  }
}
