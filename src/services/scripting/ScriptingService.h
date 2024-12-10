#ifndef SCRIPTING_SERVICE_H
#define SCRIPTING_SERVICE_H

#include <string>
#include <lua.hpp>

#include "utilities/logs_manager/LogsManager.h"

class ScriptingService {
public:
  ScriptingService(LogsManager& logsManager);
  ~ScriptingService();

  bool loadScript(const std::string& scriptPath);
  bool validateScript(const std::string& scriptPath);
  void onEnter();
  void onExit();
  void update();
  void render();

private:
  lua_State* luaState;
  
  LogsManager& logsManager;
  
  void handleLuaError(int errorCode);
};

#endif