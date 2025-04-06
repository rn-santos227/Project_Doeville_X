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

#include "factories/state/GameStateFactory.h"
#include "states/GameStateManager.h"
#include "utilities/logs_manager/LogsManager.h"

using namespace Project::Factories;
using namespace Project::States;
using namespace Project::Utilities;

namespace Project::Services {

  class ScriptingService {
  public:
    explicit ScriptingService(SDL_Renderer* renderer, GameStateManager& gameStateManager, LogsManager& logsManager);
    ~ScriptingService();

    bool validateScript(const std::string& scriptPath);
    void loadScriptsFromFolder(const std::string& folderPath);

  private:
    lua_State* luaState;
    SDL_Renderer* renderer; 

    GameStateManager& gameStateManager;
    LogsManager& logsManager;

    GameStateFactory gameStateFactory;
    
    ScriptCategory determineScriptType(const std::string& scriptName);
    
    void loadScriptByCategory(const std::string& scriptPath, ScriptCategory category);
    void handleLuaError(int errorCode);
  };
}

#endif