#ifndef SCRIPTING_SERVICE_H
#define SCRIPTING_SERVICE_H

#include "ScriptingCategory.h"

#include <iostream>
#include <regex>
#include <filesystem>
#include <fstream>
#include <string>

#include <SDL2/SDL.h>

#include "core/SDLManager.h"
#include "factories/component/ComponentsFactory.h"
#include "factories/entity/EntitiesFactory.h"
#include "factories/layer/LayersFactory.h"
#include "factories/state/GameStateFactory.h"
#include "handlers/resources/ResourcesHandler.h"
#include "states/GameStateManager.h"
#include "utilities/logs_manager/LogsManager.h"
#include "utilities/lua_state_wrapper/LuaStateWrapper.h"

namespace Project::Services {

  class ScriptingService {
  public:
    explicit ScriptingService(SDL_Renderer* renderer,
      Project::Core::SDLManager& sdlManager,
      Project::Utilities::LogsManager& logsManager, 
      Project::Handlers::ResourcesHandler& resourcesHandler, 
      Project::Factories::ComponentsFactory& componentsFactory, 
      Project::States::GameStateManager& gameStateManager);
    ~ScriptingService() = default;

    bool validateScript(const std::string& scriptPath);
    void loadScriptsFromFolder(const std::string& folderPath);

  private:
    Project::Core::SDLManager& sdlManager;
    Project::Utilities::LogsManager& logsManager;
    Project::Factories::ComponentsFactory& componentsFactory;
    Project::States::GameStateManager& gameStateManager;
    Project::Handlers::ResourcesHandler& resourcesHandler;

    Project::Utilities::LuaStateWrapper luaStateWrapper;
    SDL_Renderer* renderer;

    Project::Factories::EntitiesFactory entitiesFactory;
    Project::Factories::LayersFactory layersFactory;
    Project::Factories::GameStateFactory gameStateFactory;

    ScriptCategory determineScriptType(const std::string& scriptName);
    void loadScriptByCategory(const std::string& scriptPath, ScriptCategory category);
    void handleLuaFailure();
  };
}

#endif
