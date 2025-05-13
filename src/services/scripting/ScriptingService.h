#ifndef SCRIPTING_SERVICE_H
#define SCRIPTING_SERVICE_H

#include "ScriptingCategory.h"

#include <iostream>
#include <regex>
#include <filesystem>
#include <fstream>
#include <string>

#include <SDL2/SDL.h>

extern "C" {
  #include "lua.h"
  #include "lauxlib.h"
  #include "lualib.h"
}

#include "factories/component/ComponentsFactory.h"
#include "factories/entity/EntitiesFactory.h"
#include "factories/state/GameStateFactory.h"

#include "states/GameStateManager.h"
#include "utilities/logs_manager/LogsManager.h"

using namespace Project::Factories;
using namespace Project::States;
using namespace Project::Utilities;

namespace Project::Services {

  class ScriptingService {
  public:
    explicit ScriptingService(SDL_Renderer* renderer, LogsManager& logsManager, ComponentsFactory& componentsFactory, GameStateManager& gameStateManager);
    ~ScriptingService();

    bool validateScript(const std::string& scriptPath);
    void loadScriptsFromFolder(const std::string& folderPath);

  private:
    lua_State* luaState;
    SDL_Renderer* renderer;

    ComponentsFactory& componentsFactory;
    GameStateManager& gameStateManager;
    LogsManager& logsManager;

    EntitiesFactory entitiesFactory;
    GameStateFactory gameStateFactory;
    
    ScriptCategory determineScriptType(const std::string& scriptName);
    
    void loadScriptByCategory(const std::string& scriptPath, ScriptCategory category);
    void handleLuaError(int errorCode);
  };
}

#endif