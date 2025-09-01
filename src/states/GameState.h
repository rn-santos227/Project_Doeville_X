#ifndef GAME_STATE_H
#define GAME_STATE_H

#include "GameStateData.h"

#include <future>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "components/camera_component/CameraComponent.h"
#include "entities/ChunkSize.h"
#include "entities/EntitiesManager.h"
#include "entities/EntitySeeder.h"
#include "handlers/resources/ResourcesHandler.h"
#include "handlers/tile/TileHandler.h"
#include "interfaces/update_interface/Updatable.h"
#include "interfaces/reset_interface/Resetable.h"
#include "interfaces/render_interface/Renderable.h"
#include "layers/LayersManager.h"
#include "libraries/constants/Constants.h"
#include "platform/Platform.h"
#include "utilities/binary_cache/BinaryFileCache.h"
#include "utilities/lua_scriptable/LuaScriptable.h"

namespace Project::Factories { class EntitiesFactory; }
namespace Project::Platform { class Platform; }

namespace Project::States {
  class GameStateManager;
  class GameState : 
  public Project::Utilities::LuaScriptable, 
  public Project::Interfaces::Renderable,
  public Project::Interfaces::Resetable,
  public Project::Interfaces::Updatable {
  public:
    explicit GameState(SDL_Renderer* renderer, Project::Utilities::LogsManager& logsManager, Project::Handlers::ResourcesHandler& resourcesHandler);
    virtual ~GameState();

    virtual void initialize();
    virtual void onEnter();
    virtual void onExit();
    virtual void update(float deltaTime);
    virtual void render();
    virtual void renderDebug();
    virtual void reset();
    virtual void handleInput();
    
    virtual std::string getStateName() const { return data.stateName; }
    virtual void setStateName(const std::string stateName) { data.stateName = stateName; }

    virtual bool isActive() const { return data.active; }
    virtual void setActive(bool active) { data.active = active; }

    GameStateCategory getGameStateCategory() const { return data.gameStateCategory; }
    void setGameStateCategory(GameStateCategory category) { data.gameStateCategory = category; }

    DimensionMode getDimensionMode() const { return data.dimensionMode; }
    void setDimensionMode(DimensionMode dimension) { data.dimensionMode = dimension; }

    bool isInitialized() const { return data.initialized; }
    void markInitialized() { data.initialized = true; }

    bool attachLuaScript(const std::string& scriptPath);
    void registerLuaFunctions(Project::Platform::Platform* platform = nullptr);
    void clearScriptFunctionCache();

    void registerEntityScriptOverride(const std::string& entityName, const std::string& scriptPath);
    void applyEntityScriptOverride(Project::Entities::Entity* entity) const;
    void removeEntityScriptOverride(const std::string& entityName);
    void clearEntityScriptOverrides();
    
    bool setBackgroundImage(const std::string& imagePath);
    void setBackgroundColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a);

    float getDarkness() const { return data.darkness; }
    void setDarkness(float value);
    void setLayerDarkness(const std::string& name, float value);

    void setTimeDarknessLayer(const std::string& name);

    float getTimeOfDay() const { return data.timeOfDay; }
    void setTimeOfDay(float minutes) { 
      data.timeOfDay = minutes;
      updateDayNightCycle(0.0f);
    }

    float getDayLapse() const { return data.dayLapseSeconds; }
    void setDayLapse(float seconds) {
      data.dayLapseSeconds = (seconds > 0.0f) ? seconds : Project::Libraries::Constants::DEFAULT_DAY_LAPSE_SECONDS;
    }

    bool isTimeCycleActive() const { return data.timeCycleActive; }
    void setTimeCycleActive(bool active) { data.timeCycleActive = active; }

    void setPlatform(Project::Platform::Platform* platformPtr) { platform = platformPtr; }
    Project::Platform::Platform* getPlatform() const { return platform; }
    SDL_Renderer* getRenderer() const { return renderer; }

    void setActiveCamera(Project::Components::CameraComponent* camera);
    Project::Components::CameraComponent* getActiveCamera() const { return activeCamera; }

    void setEntitiesManager(std::shared_ptr<Project::Entities::EntitiesManager> manager) {
      entitiesManager = std::move(manager);
      if (entitiesManager) entitiesManager->setGameState(this);
    }
    Project::Entities::EntitiesManager* getEntitiesManager() const { return entitiesManager.get(); }

    void setEntitiesFactory(Project::Factories::EntitiesFactory* factory) { entitiesFactory = factory; }
    Project::Factories::EntitiesFactory* getEntitiesFactory() const { return entitiesFactory; }

    void setGlobalEntitiesManager(std::shared_ptr<Project::Entities::EntitiesManager> manager) { globalEntitiesManager = std::move(manager); }
    Project::Entities::EntitiesManager* getGlobalEntitiesManager() const { return globalEntitiesManager.get(); }

    void setGameStateManager(Project::States::GameStateManager* manager) { gameStateManager = manager; }
    Project::States::GameStateManager* getGameStateManager() const { return gameStateManager; }

    void setPlayerEntity(const std::string& name, float x = 0.0f, float y = 0.0f, bool setPosition = false);
    std::shared_ptr<Project::Entities::Entity> getPlayerEntity() const;
     
    void setMapSize(int width, int height) {
      data.mapRect.x = 0;
      data.mapRect.y = 0;
      data.mapRect.w = width;
      data.mapRect.h = height;
    }
    const SDL_Rect& getMapRect() const { return data.mapRect; }

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

    std::string startEntitySeeder(
      const std::string& seed = Project::Libraries::Constants::EMPTY_STRING,
      const std::string& layer =  Project::Libraries::Constants::EMPTY_STRING,
      const std::string& id =  Project::Libraries::Constants::EMPTY_STRING,
      Project::Entities::ChunkSize chunkSize = {}
    );

    std::string addEntitySeeder(std::unique_ptr<Project::Entities::EntitySeeder> seeder, const std::string& id =  Project::Libraries::Constants::EMPTY_STRING);
    void addEntityToSeed(
      const std::string& name, 
      const std::string& seederId = Project::Libraries::Constants::EMPTY_STRING,
      size_t count = Project::Libraries::Constants::INT_ONE
    );

    size_t getEntityCount() const;

    void setMapTiles(std::vector<Project::Handlers::BuiltTile>&& tiles, int x = 0, int y = 0, int width = 0, int height = 0);
    const std::vector<Project::Handlers::BuiltTile>& getMapTiles() const { return mapTiles; }

  protected:
    static std::unordered_map<std::string, std::vector<std::string>> scriptFunctionCache;
    static Project::Utilities::BinaryFileCache persistentFunctionCache;
    GameStateData data;

    Project::Handlers::ResourcesHandler& resourcesHandler;

    std::weak_ptr<Project::Entities::Entity> playerEntity;
    
    Project::Components::CameraComponent* activeCamera = nullptr;
    Project::Factories::EntitiesFactory* entitiesFactory = nullptr;
    Project::Platform::Platform* platform = nullptr;
    Project::States::GameStateManager* gameStateManager = nullptr;
    
    SDL_Texture* backgroundTexture = nullptr;
    SDL_Renderer* renderer = nullptr;
    
    std::future<SDL_Texture*> backgroundFuture;
    std::string pendingBackgroundPath;
    std::string luaScriptPath;

    size_t nextSeederId = 0;
    size_t seedIndex = 0;

    std::string lastSeederId;
    std::vector<std::string> seederSeeds;

    bool useImageBackground = false;

    std::shared_ptr<Project::Entities::EntitiesManager> entitiesManager;
    std::shared_ptr<Project::Entities::EntitiesManager> globalEntitiesManager;
    std::unique_ptr<Project::Layers::LayersManager> layersManager;
    std::unordered_map<std::string, std::unique_ptr<Project::Entities::EntitySeeder>> entitySeeders;
    std::unordered_map<std::string, std::string> entityScriptOverrides;
    std::vector<Project::Handlers::BuiltTile> mapTiles;

    void ensureMapSize();
    void updateDayNightCycle(float deltaTime);
  };
}

#endif
