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
}