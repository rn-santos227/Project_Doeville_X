#ifndef GAME_STATE_H
#define GAME_STATE_H

#include <string>
#include <memory>

extern "C" {
  #include "lua.h"
  #include "lauxlib.h"
  #include "lualib.h"
}

#include "entities/EntitiesManager.h"
#include "utilities/logs_manager/LogsManager.h"

using namespace Project::Entities;
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
    virtual void handleInput();
    
    virtual std::string getStateName() const { return stateName; }
    virtual void setStateName(const std::string stateName) { this->stateName = stateName; }

    virtual bool isActive() const { return active; }
    virtual void setActive(bool active) { this->active = active; }

    bool isInitialized() const { return initialized; }
    void markInitialized() { initialized = true; }

    bool attachLuaScript(const std::string& scriptPath);
    bool callLuaFunction(const std::string& functionName);

    bool setBackgroundImage(const std::string& imagePath);
    void setBackgroundColor(Uint8 r, Uint8 g, Uint8 b);

  protected:
    lua_State* luaState = nullptr;
    std::string stateName;

    SDL_Renderer* renderer = nullptr;
    SDL_Texture* backgroundTexture = nullptr;
    
    LogsManager& logsManager;
    bool initialized = false;
    bool active = false;

    std::shared_ptr<EntitiesManager> entitiesManager;
    void handleLuaError(const std::string& errorMessage);
  };
}

#endif
