#include "GameStateFactory.h"

#include <lua.hpp>
#include <string>

#include "bindings/LuaBindings.h"
#include "factories/layer/LayersFactory.h"
#include "libraries/categories/Categories.h"
#include "libraries/keys/Keys.h"
#include "layers/LayersManager.h"
#include "layers/LayerCategory.h"
#include "states/DimensionModeResolver.h"

namespace Project::Factories {
  using Project::Utilities::LogsManager;
  using Project::Handlers::ResourcesHandler;
  using Project::Factories::EntitiesFactory;
  using Project::Factories::LayersFactory;
  using Project::Platform::Platform;
  using Project::States::DimensionMode;
  using Project::States::DimensionModeResolver;
  using Project::States::GameState;
  using Project::States::GameStateCategory;
  using Project::States::GameStateCategoryResolver;
  using Project::States::GameStateManager;

  namespace Keys = Project::Libraries::Keys;
  namespace Layers = Project::Libraries::Categories::Layers;

  GameStateFactory::GameStateFactory(LogsManager& logsManager, Platform& platform, ResourcesHandler& resourcesHandler, GameStateManager& gameStateManager, EntitiesFactory& entitiesFactory, LayersFactory& layersFactory)
    : logsManager(logsManager), platform(platform), gameStateManager(gameStateManager), resourcesHandler(resourcesHandler), entitiesFactory(entitiesFactory), layersFactory(layersFactory) {}

  bool GameStateFactory::createStateFromLua(SDL_Renderer* renderer, const std::string& scriptPath) {
    auto newState = std::make_unique<GameState>(renderer, logsManager, resourcesHandler);

    if (!newState->attachLuaScript(scriptPath)) {
      logsManager.logError("Failed to load GameState from Lua script: " + scriptPath);
      return false;
    }

    newState->setPlatform(&platform);
    newState->registerLuaFunctions(&platform);
    
    lua_State* L = newState->getLuaState();
    lua_getglobal(L, Keys::STATE_NAME);

    if (!lua_isstring(L, -1)) {
      logsManager.logError("Lua script is missing 'stateName': " + scriptPath);
      return false;
    }

    std::string stateName = lua_tostring(L, -1);
    newState->setStateName(stateName);
    lua_pop(L, 1);

    lua_getglobal(L, Keys::DIMENSION);
    if (lua_isstring(L, -1)) {
      std::string typeStr = lua_tostring(L, -1);
      newState->setDimensionMode(Project::States::DimensionModeResolver::resolve(typeStr));
    } else {
      GameStateCategory cat = newState->getGameStateCategory();
      DimensionMode def = DimensionMode::BOXED;
      switch (cat) {
        case GameStateCategory::STATIC_GAME_STATE:
        case GameStateCategory::DYNAMIC_GAME_STATE:
        case GameStateCategory::CINEMATIC_STATE:
        case GameStateCategory::FPS_GAMEPLAY_STATE:
        case GameStateCategory::SIDE_SCROLLER_STATE:
        case GameStateCategory::VR_GAMEPLAY_STATE:
        case GameStateCategory::BATTLE_STATE:
        case GameStateCategory::MAP_STATE:
        case GameStateCategory::STORY_STATE:
          def = DimensionMode::FREE_ROAMING;
          break;
        default:
          def = DimensionMode::BOXED;
      }
      newState->setDimensionMode(def);
    }
    lua_pop(L, 1);

    lua_getglobal(L, Keys::STATE_CATEGORY);
    if (lua_isstring(L, -1)) {
      std::string categoryStr = lua_tostring(L, -1);
      newState->setGameStateCategory(Project::States::GameStateCategoryResolver::resolve(categoryStr));
    } else {
      logsManager.logWarning("Lua script is missing 'stateCategory', defaulting to DEBUG_STATE: " + scriptPath);
      newState->setGameStateCategory(GameStateCategory::DEBUG_STATE);
    }
    lua_pop(L, 1);

    auto entitiesMgr = std::make_shared<Project::Entities::EntitiesManager>();
    entitiesMgr->setPlatform(&platform);
    entitiesMgr->setLogsManager(&logsManager);
    newState->setEntitiesManager(std::move(entitiesMgr));
    newState->setGlobalEntitiesManager(gameStateManager.getGlobalEntitiesManager());
    newState->setEntitiesFactory(&entitiesFactory);
    newState->setGameStateManager(&gameStateManager);

    auto layersManager = std::make_unique<Project::Layers::LayersManager>();
    layersManager->setLogsManager(&logsManager);
    bool hasLayers = false;

    lua_getglobal(L, Keys::LAYER_SCRIPTS);
    if (lua_istable(L, -1)) {
      size_t len = lua_rawlen(L, -1);
      for (size_t i = 1; i <= len; ++i) {
        lua_rawgeti(L, -1, i);
        const char* path = lua_tostring(L, -1);
        if (path) {
          auto layer = layersFactory.cloneLayerFromPath(path);
          if (layer) {
            layersManager->addLayer(std::move(*layer));
            hasLayers = true;
          }
        }
        lua_pop(L, 1);
      }
    }
    lua_pop(L, 1);

    if (hasLayers) {
      newState->setLayersManager(std::move(layersManager));
    }
    gameStateManager.addState(stateName, std::move(newState));
    gameStateManager.pushState(stateName);
    logsManager.logMessage("Successfully created and registered state: " + stateName);

    return true;
  }
}
