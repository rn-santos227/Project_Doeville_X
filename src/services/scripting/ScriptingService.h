#ifndef SCRIPTING_SERVICE_H
#define SCRIPTING_SERVICE_H

#include <iostream>
#include <string>

extern "C" {
  #include "lua.h"
  #include "lauxlib.h"
  #include "lualib.h"
}

#include "utilities/logs_manager/LogsManager.h"

namespace Project::Services {

  class ScriptingService {
  public:
    explicit ScriptingService(Project::Utilities::LogsManager& logsManager);
    ~ScriptingService();

    bool loadScript(const std::string& scriptPath);
    bool validateScript(const std::string& scriptPath);

    void loadScriptsFromFolder(const std::string& folderPath);
    void registerEngineFunctions();
    
    void onEnter();
    void onExit();
    void update();
    void render();

  private:
    lua_State* luaState;
    
    Project::Utilities::LogsManager& logsManager;
    
    void handleLuaError(int errorCode);
  };
}

#endif