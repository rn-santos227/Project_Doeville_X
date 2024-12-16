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
    explicit GameState(LogsManager& logsManager)
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
      if (luaL_dofile(luaState, scriptPath.c_str()) != LUA_OK) {
        handleLuaError("Failed to load Lua script: " + scriptPath);
        return false;
      }

      logsManager.logMessage("Lua script attached: " + scriptPath);
      return true;
    }

    bool callLuaFunction(const std::string& functionName) {
      lua_getglobal(luaState, functionName.c_str());

      if (!lua_isfunction(luaState, -1)) {
        handleLuaError("Lua function not found: " + functionName);
        lua_pop(luaState, 1);
        return false;
      }

      if (lua_pcall(luaState, 0, 0, 0) != LUA_OK) {
        handleLuaError("Error calling Lua function '" + functionName + "': " + std::string(lua_tostring(luaState, -1)));
        lua_pop(luaState, 1);
        return false;
      }

      logsManager.logMessage("Lua function called successfully: " + functionName);
      return true;
    }

  protected:
    LogsManager& logsManager;
    lua_State* luaState = nullptr;
    
    bool initialized = false;

    void handleLuaError(const std::string& errorMessage) {
      logsManager.logError(errorMessage);

      if (lua_gettop(luaState) > 0 && lua_isstring(luaState, -1)) {
        logsManager.logError("Lua error details: " + std::string(lua_tostring(luaState, -1)));
        lua_pop(luaState, 1);
      }
    }
  };
}

#endif