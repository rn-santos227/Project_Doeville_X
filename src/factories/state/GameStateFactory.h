#ifndef STATE_FACTORY_H
#define STATE_FACTORY_H

#include "states/GameState.h"
#include "states/GameStateManager.h"

#include "utilities/logs_manager/LogsManager.h"

using namespace Project::States;
using namespace Project::Utilities;

namespace Project::Factories {
  class GameStateFactory {
  public:
    explicit GameStateFactory(GameStateManager& gameStateManager, LogsManager& logsManager);
  
  private:
    GameStateManager& gameStateManager;
    LogsManager& logsManager;
  };
}

#endif