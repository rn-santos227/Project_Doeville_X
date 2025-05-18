#include "LuaStateWrapper.h"

namespace Project::Utilities {
  LuaStateWrapper::LuaStateWrapper(LogsManager& logsManager) : luaState(luaL_newstate()), logsManager(logsManager) {
    if (logsManager.checkAndLogError(!luaState, "Failed to create Lua state")) {
      logsManager.flushLogs();
    } else {
      luaL_openlibs(luaState);  
    }
  }

  LuaStateWrapper::~LuaStateWrapper() {
    if (luaState) {
      lua_close(luaState);
    }
  }

  lua_State* LuaStateWrapper::get() const {
    return luaState;
  }

  bool LuaStateWrapper::isValid() const {
    return luaState != nullptr;
  }

  bool LuaStateWrapper::loadScript(const std::string& scriptPath) {
    if(logsManager.checkAndLogError(isValid(), "Lua state is invalid. Cannot load script: " + scriptPath)) {
      logsManager.flushLogs();
      return false;
    }

    if (luaL_dofile(luaState, scriptPath.c_str()) != LUA_OK) {
      handleLuaError("Failed to load Lua script: " + scriptPath);
      return false;
    }

    return true;
  }

  void LuaStateWrapper::handleLuaError(int errorCode) const {
    if (!luaState) return; 
    const char* errorMessage = lua_tostring(luaState, -1);
    logsManager.logError("Lua Error [" + std::to_string(errorCode) + "]: " + std::string(errorMessage));
    lua_pop(luaState, 1);
  }

  void LuaStateWrapper::handleLuaError(const std::string& errorMessage) const {
    logsManager.logError(errorMessage);
    if (luaState && lua_gettop(luaState) > 0 && lua_isstring(luaState, -1)) {
      logsManager.logError("Lua error details: " + std::string(lua_tostring(luaState, -1)));
      lua_pop(luaState, 1);
    }
  }
}