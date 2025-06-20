#ifndef SCRIPTING_SERVICE_H
#define SCRIPTING_SERVICE_H

#include "ScriptingCategory.h"

#include <iostream>
#include <regex>
#include <filesystem>
#include <fstream>
#include <string>

#include <SDL2/SDL.h>

#include "factories/component/ComponentsFactory.h"
#include "factories/entity/EntitiesFactory.h"
#include "factories/state/GameStateFactory.h"

#include "handlers/resources/ResourcesHandler.h"

#include "states/GameStateManager.h"

#include "utilities/logs_manager/LogsManager.h"
#include "utilities/lua_state_wrapper/LuaStateWrapper.h"

using namespace Project::Factories;
using namespace Project::Handlers;
using namespace Project::States;
using namespace Project::Utilities;

namespace Project::Services {

  class ScriptingService {
  public:
    explicit ScriptingService(SDL_Renderer* renderer, LogsManager& logsManager, ResourcesHandler& resourcesHandler, ComponentsFactory& componentsFactory, GameStateManager& gameStateManager);
    ~ScriptingService() = default;

    bool validateScript(const std::string& scriptPath);
    void loadScriptsFromFolder(const std::string& folderPath);

  private:
    LogsManager& logsManager;
    ComponentsFactory& componentsFactory;
    GameStateManager& gameStateManager;
    ResourcesHandler& resourcesHandler;

    LuaStateWrapper luaStateWrapper;
    SDL_Renderer* renderer;

    EntitiesFactory entitiesFactory;
    GameStateFactory gameStateFactory;
    
    ScriptCategory determineScriptType(const std::string& scriptName);
    void loadScriptByCategory(const std::string& scriptPath, ScriptCategory category);
  };
}
#endif
