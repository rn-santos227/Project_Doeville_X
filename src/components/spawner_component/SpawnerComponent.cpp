#include "SpawnerComponent.h"

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

  void SpawnerComponent::build(Project::Utilities::LuaStateWrapper& luaStateWrapper, const std::string& tableName) {
    templateName = luaStateWrapper.getTableString(tableName, Keys::TEMPLATE, Constants::EMPTY_STRING);
  }

  void SpawnerComponent::spawn(float _offsetX, float _offsetY, float _velocityX, float _velocityY) {
    if (!owner) return;
    
    EntitiesManager* manager = owner->getEntitiesManager();
    if (!manager) return;
    
    GameState* state = manager->getGameState();
    if (!state) return;
    
    EntitiesFactory* factory = state->getEntitiesFactory();
    if (!factory) return;

    if (templateName.empty()) return;

    std::unique_ptr<Entity> ent = factory->cloneEntity(templateName);
    if (!ent) return;

    float x = owner->getX() + _offsetX;
    float y = owner->getY() + _offsetY;
    ent->getLuaStateWrapper().setGlobalNumber(Keys::X, x);
    ent->getLuaStateWrapper().setGlobalNumber(Keys::Y, y);
    ent->initialize();

    if (auto* physics = dynamic_cast<PhysicsComponent*>(ent->getComponent(Components::PHYSICS_COMPONENT))) {
      physics->setVelocity(_velocityX, _velocityY);
    }

    std::shared_ptr<Entity> shared = std::move(ent);
    manager->addEntity(shared);
  }
}
