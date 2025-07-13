#include "GameStateFactory.h"

#include <lua.hpp>
#include <string>

#include "factories/layer/LayersFactory.h"
#include "libraries/keys/Keys.h"
#include "layers/LayersManager.h"
#include "layers/LayerCategory.h"

namespace Project::Factories {
  using Project::Utilities::LogsManager;
  using Project::Handlers::ResourcesHandler;
  using Project::States::GameState;
  using Project::States::GameStateCategory;
  using Project::States::GameStateCategoryResolver;
  using Project::States::GameStateManager;
  using Project::Factories::EntitiesFactory;

  namespace Keys = Project::Libraries::Keys;

  GameStateFactory::GameStateFactory(LogsManager& logsManager, ResourcesHandler& resourcesHandler, GameStateManager& gameStateManager, EntitiesFactory& entitiesFactory)
  : logsManager(logsManager), gameStateManager(gameStateManager), resourcesHandler(resourcesHandler), entitiesFactory(entitiesFactory) {}

  bool GameStateFactory::createStateFromLua(SDL_Renderer* renderer, const std::string& scriptPath) {
    auto newState = std::make_unique<GameState>(renderer, logsManager, resourcesHandler);

    if (!newState->attachLuaScript(scriptPath)) {
      logsManager.logError("Failed to load GameState from Lua script: " + scriptPath);
      return false;
    }
    
    lua_State* L = newState->getLuaState();
    lua_getglobal(L, Keys::STATE_NAME);

    if (!lua_isstring(L, -1)) {
      logsManager.logError("Lua script is missing 'stateName': " + scriptPath);
      return false;
    }

    std::string stateName = lua_tostring(L, -1);
    newState->setStateName(stateName);
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

    newState->setEntitiesManager(std::make_shared<Project::Entities::EntitiesManager>());
    newState->setGlobalEntitiesManager(gameStateManager.getGlobalEntitiesManager());
    newState->setEntitiesFactory(&entitiesFactory);
    newState->setGameStateManager(&gameStateManager);

    auto layersManager = std::make_unique<Project::Layers::LayersManager>();

    gameStateManager.addState(stateName, std::move(newState));
    gameStateManager.pushState(stateName);
    logsManager.logMessage("Successfully created and registered state: " + stateName);

    return true;
  }
}
