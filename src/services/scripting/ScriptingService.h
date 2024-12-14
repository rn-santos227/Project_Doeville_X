#ifndef SCRIPTING_SERVICE_H
#define SCRIPTING_SERVICE_H

#include "ScriptingCategory.h"

#include <iostream>
#include <regex>
#include <filesystem>
#include <string>

extern "C" {
  #include "lua.h"
  #include "lauxlib.h"
  #include "lualib.h"
}

#include "utilities/logs_manager/LogsManager.h"

using namespace Project::Utilities;

namespace Project::Services {

  class ScriptingService {
  public:
    explicit ScriptingService(LogsManager& logsManager);
    ~ScriptingService();

    bool loadScript(const std::string& scriptPath);
    bool validateScript(const std::string& scriptPath);

    void loadScriptsFromFolder(const std::string& folderPath);
    void registerEngineFunctions();

  private:
    lua_State* luaState;
    
    LogsManager& logsManager;
    
    void handleLuaError(int errorCode);
  };
}

#endif