#ifndef GAME_STATE_H
#define GAME_STATE_H

#include "GameState.h"

#include <string>

extern "C" {
  #include "lua.h"
  #include "lauxlib.h"
  #include "lualib.h"
}

namespace Project::States {
  class GameState {
  public:
    virtual ~GameState() = default;
    
    virtual void initialize() = 0;

    virtual void onEnter() = 0;
    
    virtual void onExit() = 0;
    
    virtual void update(float deltaTime) = 0;
    
    virtual void render() = 0;

    virtual void handleInput() = 0;
    
    virtual std::string getName() const {
      return name;
    }

    bool isInitialized() const { return initialized; }
    void markInitialized() { initialized = true; }

  protected:
    std::string name;
    bool initialized = false;

    lua_State* luaState = nullptr;
    std::string luaName;
    
    void reportLuaError() {

    }
  };
}

#endif