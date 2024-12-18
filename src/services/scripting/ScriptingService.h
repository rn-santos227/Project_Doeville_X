#ifndef SCRIPTING_SERVICE_H
#define SCRIPTING_SERVICE_H

#include "ScriptingCategory.h"

#include <iostream>
#include <regex>
#include <filesystem>
#include <fstream>
#include <string>

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
    explicit ScriptingService(GameStateFactory& gameStateFactory, LogsManager& logsManager);
    ~ScriptingService();

    bool loadScript(const std::string& scriptPath);
    bool validateScript(const std::string& scriptPath);
    void loadScriptsFromFolder(const std::string& folderPath);

  private:
    lua_State* luaState;

    GameStateFactory& gameStateFactory;

    LogsManager& logsManager;
    ScriptCategory determineScriptType(const std::string& scriptName);
    
    void loadScriptByCategory(const std::string& scriptPath, ScriptCategory category);
    void handleLuaError(int errorCode);
  };
}

#endif