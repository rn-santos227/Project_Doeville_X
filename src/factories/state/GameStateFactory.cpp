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
    
    lua_State* L = newState->getLuaState();
    lua_getglobal(L, "stateName");

    if (!lua_isstring(L, -1)) {
      logsManager.logError("Lua script is missing 'stateName': " + scriptPath);
      return false;
    }

    std::string stateName = lua_tostring(L, -1);
    newState->setStateName(stateName);
    lua_pop(L, 1);

    gameStateManager.addState(stateName, std::move(newState));
    gameStateManager.pushState(stateName);
    logsManager.logMessage("Successfully created and registered state: " + stateName);

    return true;
  }
}