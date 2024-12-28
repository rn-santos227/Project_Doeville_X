#include "EntityFactory.h"

namespace Project::Factories {
  EntityFactory::EntityFactory(EntitiesManager& entitiesManager, LogsManager& logsManager)
  : entitiesManager(entitiesManager), logsManager(logsManager) {}

  EntityFactory::~EntityFactory() {
    entitiesManager.reset();
  }

  bool EntityFactory::createEntityFromLua(const std::string& scriptPath) {
    auto entity = std::make_unique<Entity>(logsManager);

    if (!entity->attachLuaScript(scriptPath)) {
      logsManager.logError("Failed to load Entity from Lua script: " + scriptPath);
      return false;
    }

    return true;
  }
}