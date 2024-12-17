#include "GameStateFactory.h"
namespace Project::Factories {
  GameStateFactory::GameStateFactory(GameStateManager& gameStateManager, LogsManager& logsManager)
  : gameStateManager(gameStateManager), logsManager(logsManager) {}

  bool GameStateFactory::createStateFromLua(const std::string& scriptPath) {
    auto newState = std::make_unique<GameState>(logsManager);

    if (!newState->attachLuaScript(scriptPath)) {
      logsManager.logError("Failed to load GameState from Lua script: " + scriptPath);
      return false;
    }
  }
}