#ifndef STATE_FACTORY_H
#define STATE_FACTORY_H

#include "states/GameState.h"
#include "states/GameStateManager.h"

#include "utilities/logs_manager/LogsManager.h"

#include <SDL2/SDL.h>

using namespace Project::States;
using namespace Project::Utilities;

namespace Project::Factories {
  class GameStateFactory {
  public:
    explicit GameStateFactory(LogsManager& logsManager, GameStateManager& gameStateManager);

    bool createStateFromLua(SDL_Renderer* renderer, const std::string& scriptPath);
  
  private:
    LogsManager& logsManager;
    GameStateManager& gameStateManager;
  };
}

#endif