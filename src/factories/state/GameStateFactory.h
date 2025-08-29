#ifndef STATE_FACTORY_H
#define STATE_FACTORY_H

#include <SDL2/SDL.h>

#include "factories/entity/EntitiesFactory.h"
#include "factories/layer/LayersFactory.h"
#include "handlers/resources/ResourcesHandler.h"
#include "platform/Platform.h"
#include "states/GameState.h"
#include "states/GameStateCategory.h"
#include "states/GameStateCategoryResolver.h"
#include "states/GameStateManager.h"
#include "utilities/logs_manager/LogsManager.h"

namespace Project::Factories {
  class GameStateFactory {
  public:
    explicit GameStateFactory(
      Project::Utilities::LogsManager& logsManager,
      Project::Platform::Platform& platform,
      Project::Handlers::ResourcesHandler& resourcesHandler,
      Project::States::GameStateManager& gameStateManager,
      Project::Factories::EntitiesFactory& entitiesFactory,
      Project::Factories::LayersFactory& layersFactory
    );

    bool createStateFromLua(SDL_Renderer* renderer, const std::string& scriptPath);
  
  private:
    Project::Utilities::LogsManager& logsManager;
    Project::Handlers::ResourcesHandler& resourcesHandler;
    Project::States::GameStateManager& gameStateManager;
    Project::Factories::EntitiesFactory& entitiesFactory;
    Project::Factories::LayersFactory& layersFactory;
    Project::Platform::Platform& platform;
  };
}

#endif
