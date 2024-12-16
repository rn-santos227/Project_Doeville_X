#include "GameState.h"

namespace Project::States {
  GameState::GameState(LogsManager& logsManager)
    : logsManager(logsManager), luaState(luaL_newstate()), initialized(false) {
    luaL_openlibs(luaState);
  }

  GameState::~GameState() {
    if (luaState) {
      lua_close(luaState);
    }
  }

  bool GameState::attachLuaScript(const std::string& scriptPath) {
    if (luaL_dofile(luaState, scriptPath.c_str()) != LUA_OK) {
      handleLuaError("Failed to load Lua script: " + scriptPath);
      return false;
    }
    logsManager.logMessage("Lua script attached: " + scriptPath);
    return true;
  }

  void GameState::handleLuaError(const std::string& errorMessage) {
    logsManager.logError(errorMessage);

    if (lua_gettop(luaState) > 0 && lua_isstring(luaState, -1)) {
      logsManager.logError("Lua error details: " + std::string(lua_tostring(luaState, -1)));
      lua_pop(luaState, 1);
    }
  }
}