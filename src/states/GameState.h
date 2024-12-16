#ifndef GAME_STATE_H
#define GAME_STATE_H

#include "GameState.h"

#include <string>

extern "C" {
  #include "lua.h"
  #include "lauxlib.h"
  #include "lualib.h"
}

#include "utilities/logs_manager/LogsManager.h"

using namespace Project::Utilities;

namespace Project::States {
  class GameState {
  public:
    explicit GameState(Project::Utilities::LogsManager& logsManager)
      : logsManager(logsManager), luaState(luaL_newstate()), initialized(false) {
      luaL_openlibs(luaState);
    }

    virtual ~GameState() {
      if (luaState) {
        lua_close(luaState);
      }
    }
    
    virtual void initialize() = 0;

    virtual void onEnter() = 0;
    
    virtual void onExit() = 0;
    
    virtual void update(float deltaTime) = 0;
    
    virtual void render() = 0;

    virtual void handleInput() = 0;
    
    virtual std::string getName() const = 0;

    bool isInitialized() const { return initialized; }
    void markInitialized() { initialized = true; }

    bool attachLuaScript(const std::string& scriptPath) {

    }

  protected:
    LogsManager& logsManager;
    lua_State* luaState = nullptr;
    
    bool initialized = false;
  };
}

#endif