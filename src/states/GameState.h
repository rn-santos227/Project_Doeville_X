#ifndef GAME_STATE_H
#define GAME_STATE_H

#include <string>
#include <memory>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

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
    explicit GameState(SDL_Renderer* renderer, LogsManager& logsManager);
    virtual ~GameState();

    lua_State* getLuaState() const { return luaState; }

    virtual void initialize();
    virtual void onEnter();
    virtual void onExit();
    virtual void update(float deltaTime);
    virtual void render(SDL_Renderer* renderer);
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
    void clearBackground();

  protected:
    lua_State* luaState = nullptr;
    LogsManager& logsManager;
    std::string stateName;

    SDL_Texture* backgroundTexture = nullptr;
    SDL_Renderer* renderer = nullptr;
    SDL_Color backgroundColor = {0, 0, 0, 255};

    bool useImageBackground = false;
    bool initialized = false;
    bool active = false;

    std::shared_ptr<EntitiesManager> entitiesManager;

    int lua_setBackgroundImage(lua_State* L);
    int lua_setBackgroundColor(lua_State* L);

    void handleLuaError(const std::string& errorMessage);
  };
}

#endif
