#include "GameStateFactory.h"
namespace Project::Factories {
  GameStateFactory::GameStateFactory(GameStateManager& gameStateManager, LogsManager& logsManager)
  : gameStateManager(gameStateManager), logsManager(logsManager) {}

  
}