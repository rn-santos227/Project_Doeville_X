#ifndef GAME_STATE_H
#define GAME_STATE_H

#include "GameStateCategory.h"

#include <string>
#include <memory>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "entities/EntitiesManager.h"
#include "factories/entity/EntitiesFactory.h"
#include "libraries/constants/Constants.h"
#include "handlers/resources/ResourcesHandler.h"
#include "utilities/lua_scriptable/LuaScriptable.h"

namespace Constants = Project::Libraries::Constants;

namespace Project::States {
  class GameState : public Project::Utilities::LuaScriptable {
  public:
    explicit GameState(SDL_Renderer* renderer, Project::Utilities::LogsManager& logsManager, Project::Handlers::ResourcesHandler& resourcesHandler);
    virtual ~GameState();

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

    GameStateCategory getGameStateCategory() const { return gameStateCategory; }
    void setGameStateCategory(GameStateCategory category) { gameStateCategory = category; }

    bool isInitialized() const { return initialized; }
    void markInitialized() { initialized = true; }

    bool attachLuaScript(const std::string& scriptPath);

    bool setBackgroundImage(const std::string& imagePath);
    void setBackgroundColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a);

    void setEntitiesManager(std::shared_ptr<Project::Entities::EntitiesManager> manager) { entitiesManager = std::move(manager); }
    void setEntitiesFactory(Project::Factories::EntitiesFactory* factory) { entitiesFactory = factory; }

    void clearBackground();

    //overload functions
    void setBackgroundColor(Uint8 r, Uint8 g, Uint8 b) {
      setBackgroundColor(r, g, b, Constants::FULL_ALPHA);
    };

  protected:
    Project::Handlers::ResourcesHandler& resourcesHandler;
    GameStateCategory gameStateCategory = GameStateCategory::DEBUG_STATE;
    
    Project::Factories::EntitiesFactory* entitiesFactory = nullptr;
    SDL_Texture* backgroundTexture = nullptr;
    SDL_Renderer* renderer = nullptr;
    
    SDL_Color backgroundColor = Constants::DEFAULT_BACKGROUND_COLOR;

    std::string stateName;
    bool useImageBackground = false;
    bool initialized = false;
    bool active = false;

    std::shared_ptr<Project::Entities::EntitiesManager> entitiesManager;

    static int lua_setBackgroundImage(lua_State* L);
    static int lua_setBackgroundColor(lua_State* L);
    static int lua_spawnEntity(lua_State* L);
  };
}
#endif
