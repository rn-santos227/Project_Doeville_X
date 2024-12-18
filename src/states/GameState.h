#ifndef GAME_STATE_H
#define GAME_STATE_H

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
    explicit GameState(LogsManager& logsManager);
    virtual ~GameState();

    lua_State* getLuaState() const { return luaState; }

    virtual void initialize();
    virtual void onEnter();
    virtual void onExit();
    virtual void update(float deltaTime);
    virtual void render();
    virtual void handleInput() = 0;
    virtual std::string getName() const = 0;

    bool isInitialized() const { return initialized; }
    void markInitialized() { initialized = true; }

    bool attachLuaScript(const std::string& scriptPath);
    bool callLuaFunction(const std::string& functionName);

  protected:
    LogsManager& logsManager;
    lua_State* luaState = nullptr;
    bool initialized = false;

    void handleLuaError(const std::string& errorMessage);
  };
}

#endif
