#ifndef GAME_STATE_H
#define GAME_STATE_H

#include "GameStateCategory.h"

#include <string>
#include <memory>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "entities/EntitiesManager.h"

#include "handlers/resources/ResourcesHandler.h"

#include "utilities/logs_manager/LogsManager.h"
#include "utilities/lua_state_wrapper/LuaStateWrapper.h"

using namespace Project::Handlers;
using namespace Project::Entities;
using namespace Project::Utilities;

namespace Project::States {
  class GameState {
  public:
    explicit GameState(SDL_Renderer* renderer, LogsManager& logsManager);
    virtual ~GameState();

    lua_State* getLuaState() const { return luaStateWrapper.get(); }

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
    void setBackgroundColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a);
    void clearBackground();

    //overload functions
    void setBackgroundColor(Uint8 r, Uint8 g, Uint8 b) {
      setBackgroundColor(r, g, b, 255);
    };

  protected:
    LogsManager& logsManager;
    LuaStateWrapper luaStateWrapper;

    GameStateCategory gameStateCategory;
    
    SDL_Texture* backgroundTexture = nullptr;
    SDL_Renderer* renderer = nullptr;
    SDL_Color backgroundColor = {0, 0, 0, 255};

    std::string stateName;
    bool useImageBackground = false;
    bool initialized = false;
    bool active = false;

    std::shared_ptr<EntitiesManager> entitiesManager;

    static int lua_setBackgroundImage(lua_State* L);
    static int lua_setBackgroundColor(lua_State* L);
  };
}
#endif
