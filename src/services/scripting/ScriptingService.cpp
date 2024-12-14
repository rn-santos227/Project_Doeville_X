#include "ScriptingService.h"

namespace fs = std::filesystem;

namespace Project::Services {
  ScriptingService::ScriptingService(Project::Utilities::LogsManager& logsManager) : logsManager(logsManager) {
    luaState = luaL_newstate();
    luaL_openlibs(luaState);
  }

  ScriptingService::~ScriptingService() {
    if (luaState) {
      lua_close(luaState);
    }
  }

  void ScriptingService::loadScriptsFromFolder(const std::string& folderPath) {
    for (const auto& entry : fs::recursive_directory_iterator(folderPath)) {
      if (entry.is_regular_file()) {
        const std::string filePath = entry.path().string();
        const std::string fileName = entry.path().filename().string();
      }
    }
  }
  
  bool ScriptingService::validateScript(const std::string& scriptPath) {
    int result = luaL_loadfile(luaState, scriptPath.c_str());
    if (result != LUA_OK) {
      handleLuaError(result);
      return false;
    }
    lua_pop(luaState, 1);
    return true;
  }

  bool ScriptingService::loadScript(const std::string& scriptPath) {
    int result = luaL_dofile(luaState, scriptPath.c_str());
    if (result != LUA_OK) {
      handleLuaError(result);
      return false;
    }
    return true;
  }

  void ScriptingService::handleLuaError(int errorCode) {
    const char* errorMessage = lua_tostring(luaState, -1);
    logsManager.logError("Lua Error [" + std::to_string(errorCode) + "]:" + std::string(errorMessage));
    lua_pop(luaState, 1); 
  }
}