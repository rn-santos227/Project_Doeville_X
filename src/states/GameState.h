#ifndef GAME_STATE_H
#define GAME_STATE_H

#include "GameStateCategory.h"

#include <future>
#include <memory>
#include <string>
#include <vector>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "components/camera_component/CameraComponent.h"
#include "entities/EntitiesManager.h"
#include "entities/EntitySeeder.h"
#include "handlers/resources/ResourcesHandler.h"
#include "interfaces/update_interface/Updatable.h"
#include "interfaces/render_interface/Renderable.h"
#include "layers/LayersManager.h"
#include "libraries/constants/Constants.h"
#include "utilities/lua_scriptable/LuaScriptable.h"

namespace Project::Factories { class EntitiesFactory; }
namespace Project::States {
  class GameStateManager;
  class GameState : public Project::Utilities::LuaScriptable, public Project::Interfaces::Renderable, public Project::Interfaces::Updatable {
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

    void setActiveCamera(Project::Components::CameraComponent* camera);
    Project::Components::CameraComponent* getActiveCamera() const { return activeCamera; }

    void setEntitiesManager(std::shared_ptr<Project::Entities::EntitiesManager> manager) {
      entitiesManager = std::move(manager);
      if (entitiesManager) entitiesManager->setGameState(this);
    }

    void setGlobalEntitiesManager(std::shared_ptr<Project::Entities::EntitiesManager> manager) { globalEntitiesManager = std::move(manager); }
    void setEntitiesFactory(Project::Factories::EntitiesFactory* factory) { entitiesFactory = factory; }
    void setGameStateManager(Project::States::GameStateManager* manager) { gameStateManager = manager; }
    void setLayersManager(std::unique_ptr<Project::Layers::LayersManager> manager) {
      layersManager = std::move(manager);
      if (layersManager) layersManager->setGameState(this);
    }

    std::shared_ptr<Project::Entities::Entity> findEntity(const std::string& name);
    Project::Layers::LayersManager* getLayersManager() const { return layersManager.get(); }
    void clearBackground();

    //overload functions
    void setBackgroundColor(Uint8 r, Uint8 g, Uint8 b) {
      setBackgroundColor(r, g, b, Project::Libraries::Constants::FULL_ALPHA);
    };

  protected:
    static int lua_setActiveCamera(lua_State* L);
    static int lua_setBackgroundImage(lua_State* L);
    static int lua_setBackgroundColor(lua_State* L);
    static int lua_spawnEntity(lua_State* L);
    static int lua_changeState(lua_State* L);
    static int lua_startEntitySeeder(lua_State* L);

    void addEntitySeeder(std::unique_ptr<Project::Entities::EntitySeeder> seeder) {
      entitySeeders.emplace_back(std::move(seeder));
    }
    
    Project::Handlers::ResourcesHandler& resourcesHandler;
    GameStateCategory gameStateCategory = GameStateCategory::DEBUG_STATE;
    
    Project::Components::CameraComponent* activeCamera = nullptr;
    Project::Factories::EntitiesFactory* entitiesFactory = nullptr;
    SDL_Texture* backgroundTexture = nullptr;
    SDL_Renderer* renderer = nullptr;
    
    SDL_Color backgroundColor = Project::Libraries::Constants::DEFAULT_BACKGROUND_COLOR;
    
    std::future<SDL_Texture*> backgroundFuture;
    std::string pendingBackgroundPath;
    std::string stateName;

    bool useImageBackground = false;
    bool initialized = false;
    bool active = false;

    Project::States::GameStateManager* gameStateManager = nullptr;
    std::shared_ptr<Project::Entities::EntitiesManager> entitiesManager;
    std::shared_ptr<Project::Entities::EntitiesManager> globalEntitiesManager;
    std::unique_ptr<Project::Layers::LayersManager> layersManager;
    std::vector<std::unique_ptr<Project::Entities::EntitySeeder>> entitySeeders;

    void startEntitySeeder(const std::string& seed = "");

  };
}

#endif
