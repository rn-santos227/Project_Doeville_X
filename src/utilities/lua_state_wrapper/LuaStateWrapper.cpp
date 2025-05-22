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
      luaState = nullptr;
    }
  }

  lua_State* LuaStateWrapper::get() const {
    return luaState;
  }

  bool LuaStateWrapper::isValid() const {
    return luaState != nullptr;
  }

  bool LuaStateWrapper::loadScript(const std::string& scriptPath) {
    if(logsManager.checkAndLogError(!isValid(), "Lua state is invalid. Cannot load script: " + scriptPath)) {
      logsManager.flushLogs();
      return false;
    }

    if (luaL_dofile(luaState, scriptPath.c_str()) != LUA_OK) {
      handleLuaError("Failed to load Lua script: " + scriptPath);
      return false;
    }
    return true;
  }
  
   std::string LuaStateWrapper::getGlobalString(const std::string& name, const std::string& defaultValue) const {
    if (!isValid()) return defaultValue;

    lua_getglobal(luaState, name.c_str());
    std::string result = defaultValue;
    if (lua_isstring(luaState, -1)) {
      result = lua_tostring(luaState, -1);
    }

    lua_pop(luaState, 1);
    return result;
  }

  float LuaStateWrapper::getGlobalNumber(const std::string& name, float defaultValue) const {
    if (!isValid()) return defaultValue;
    
    lua_getglobal(luaState, name.c_str());
    float result = defaultValue;
    if (lua_isnumber(luaState, -1)) {
      result = static_cast<float>(lua_tonumber(luaState, -1));
    }

    lua_pop(luaState, 1);
    return result;
  }

  bool LuaStateWrapper::getGlobalBoolean(const std::string& name, bool defaultValue) const {
    if (!isValid()) return defaultValue;

    lua_getglobal(luaState, name.c_str());
    bool result = defaultValue;
    if (lua_isboolean(luaState, -1)) {
      result = lua_toboolean(luaState, -1);
    }

    lua_pop(luaState, 1);
    return result;
  }

  bool LuaStateWrapper::isGlobalFunction(const std::string& name) const {
    if (!isValid()) return false;
    
    lua_getglobal(luaState, name.c_str());
    bool isFunc = lua_isfunction(luaState, -1);
    lua_pop(luaState, 1);
    return isFunc;
  }

  bool LuaStateWrapper::callGlobalFunction(const std::string& name, int nargs, int nresults) const {
    if (!isValid()) return false;

    lua_getglobal(luaState, name.c_str());
    if (!lua_isfunction(luaState, -1)) {
      lua_pop(luaState, 1);
      return false;
    }

    if (lua_pcall(luaState, nargs, nresults, 0) != LUA_OK) {
      const char* error = lua_tostring(luaState, -1);
      handleLuaError(std::string("Lua function call failed: ") + (error ? error : "Unknown error"));
      lua_pop(luaState, 1);
      return false;
    }

    return true;
  }

  bool LuaStateWrapper::callFunctionIfExists(const std::string& name) {
    if (!isValid()) return false;

    lua_getglobal(luaState, name.c_str());
    if (!lua_isfunction(luaState, -1)) {

    }
  }

  bool LuaStateWrapper::isGlobalTable(const std::string& name) const {
    if (!isValid()) return false;
    lua_getglobal(luaState, name.c_str());
    bool isTable = lua_istable(luaState, -1);
    lua_pop(luaState, 1);
    return isTable;
  }

  void LuaStateWrapper::iterateGlobalTable(const std::string& name, std::function<void(lua_State*, int)> callback) const {
    if (!isValid()) return;

    lua_getglobal(luaState, name.c_str());
    if (!lua_istable(luaState, -1)) {
      lua_pop(luaState, 1);
      return;
    }

    lua_pushnil(luaState);
    while (lua_next(luaState, -2)) {
      callback(luaState, lua_gettop(luaState));
      lua_pop(luaState, 1);
    }

    lua_pop(luaState, 1);
  }

  int LuaStateWrapper::luaPrintRedirect(lua_State* L) {
    int nargs = lua_gettop(L);
    std::string output;

    for (int i = 1; i <= nargs; ++i) {
      if (i > 1) output += "\t";
      output += lua_tostring(L, i);
    }

    std::cout << "[LUA] " << output << std::endl;
    return 0;
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