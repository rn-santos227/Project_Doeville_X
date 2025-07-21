#include "SpawnerComponent.h"

#include "entities/Entity.h"
#include "entities/EntitiesManager.h"
#include "factories/entity/EntitiesFactory.h"
#include "states/GameState.h"
#include "libraries/keys/Keys.h"

namespace Project::Components {
  using Project::Entities::Entity;
  using Project::Entities::EntitiesManager;
  using Project::Factories::EntitiesFactory;
  using Project::States::GameState;

  namespace Constants = Project::Libraries::Constants;
  namespace Keys = Project::Libraries::Keys;

  void SpawnerComponent::build(Project::Utilities::LuaStateWrapper& luaStateWrapper, const std::string& tableName) {
    templateName = luaStateWrapper.getTableString(tableName, Keys::TEMPLATE, Constants::EMPTY_STRING);
  }

  void SpawnerComponent::spawn(float offsetX, float offsetY) {
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
  }
}
