#include "LuaStateWrapper.h"

namespace Project::Utilities {
  LuaStateWrapper::LuaStateWrapper(LogsManager& logsManager) : luaState(luaL_newstate()), logsManager(logsManager) {
    if (logsManager.checkAndLogError(!luaState, "Failed to create Lua state")) {
      return;
    }
    luaL_openlibs(luaState);  
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
      return false;
    }

    if (luaL_dofile(luaState, scriptPath.c_str()) != LUA_OK) {
      handleLuaError("Failed to load Lua script: " + scriptPath);
      return false;
    }
    return true;
  }

  bool LuaStateWrapper::loadScriptFromString(const std::string& code) {
    if(logsManager.checkAndLogError(!isValid(), "Lua state is invalid. Cannot load script from string.")) {
      return false;
    }

    if (luaL_dostring(luaState, code.c_str()) != LUA_OK) {
      handleLuaError("Failed to load Lua script from string.");
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

  void LuaStateWrapper::setGlobalString(const std::string& name, const std::string& value) const {
    if (!isValid()) return;
    lua_pushstring(luaState, value.c_str());
    lua_setglobal(luaState, name.c_str());
  }

  void LuaStateWrapper::setGlobalNumber(const std::string& name, float value) {
    if (!isValid()) return;
    lua_pushnumber(luaState, value);
    lua_setglobal(luaState, name.c_str());
  }

  void LuaStateWrapper::setGlobalBoolean(const std::string& name, bool value) {
    if (!isValid()) return;
    lua_pushboolean(luaState, value);
    lua_setglobal(luaState, name.c_str());
  }

   std::string LuaStateWrapper::getTableString(const std::string& tableName, const std::string& key, const std::string& defaultValue) const {
    if (!isValid()) return defaultValue;

    lua_getglobal(luaState, tableName.c_str());
    if (!lua_istable(luaState, -1)) {
      lua_pop(luaState, 1);
      return defaultValue;
    }

    lua_pushstring(luaState, key.c_str());
    lua_gettable(luaState, -2);
    std::string result = defaultValue;
    if (lua_isstring(luaState, -1)) {
      result = lua_tostring(luaState, -1);
    }

    lua_pop(luaState, 2);
    return result;
  }

  float LuaStateWrapper::getTableNumber(const std::string& tableName, const std::string& key, float defaultValue) const {
    if (!isValid()) return defaultValue;

    lua_getglobal(luaState, tableName.c_str());
    if( !lua_istable(luaState, -1)) {
      lua_pop(luaState, 1);
      return defaultValue;
    }

    lua_pushstring(luaState, key.c_str());
    lua_gettable(luaState, -2);
    float result = defaultValue;
    if (lua_isnumber(luaState, -1)) {
      result = static_cast<float>(lua_tonumber(luaState, -1));
    }

    lua_pop(luaState, 2);
    return result;
  }

  bool LuaStateWrapper::getTableBoolean(const std::string& tableName, const std::string& key, bool defaultValue) const {
    if (!isValid()) return defaultValue;

    lua_getglobal(luaState, tableName.c_str());
    if (!lua_istable(luaState, -1)) {
      lua_pop(luaState, 1);
      return defaultValue;
    }

    lua_pushstring(luaState, key.c_str());
    lua_gettable(luaState, -2);
    bool result = defaultValue;
    if (lua_isboolean(luaState, -1)) {
      result = lua_toboolean(luaState, -1);
    }

    lua_pop(luaState, 2);
    return result;
  }

  void LuaStateWrapper::setTableString(const std::string& tableName, const std::string& key, const std::string& value) {
    if (!isValid()) return;

    lua_getglobal(luaState, tableName.c_str());
    if (!lua_istable(luaState, -1)) {
      lua_pop(luaState, 1);
      handleLuaError("Table '" + tableName + "' does not exist.");
      return;
    }

    lua_pushstring(luaState, key.c_str());
    lua_pushstring(luaState, value.c_str());
    lua_settable(luaState, -3);
    lua_pop(luaState, 1);
  }

  void LuaStateWrapper::setTableNumber(const std::string& tableName, const std::string& key, float value) {
    if (!isValid()) return;

    lua_getglobal(luaState, tableName.c_str());
    if (!lua_istable(luaState, -1)) {
      lua_pop(luaState, 1);
      handleLuaError("Table '" + tableName + "' does not exist.");
      return;
    }

    lua_pushstring(luaState, key.c_str());
    lua_pushnumber(luaState, value);
    lua_settable(luaState, -3);
    lua_pop(luaState, 1);
  }

  void LuaStateWrapper::setTableBoolean(const std::string& tableName, const std::string& key, bool value) {
    if (!isValid()) return;

    lua_getglobal(luaState, tableName.c_str());
    if (!lua_istable(luaState, -1)) {
      lua_pop(luaState, 1);
      handleLuaError("Table '" + tableName + "' does not exist.");
      return;
    }

    lua_pushstring(luaState, key.c_str());
    lua_pushboolean(luaState, value);
    lua_settable(luaState, -3);
    lua_pop(luaState, 1);
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
      return false;
    }

    return true;
  }

  bool LuaStateWrapper::callFunctionIfExists(const std::string& name) {
    if (!isValid()) return false;

    lua_getglobal(luaState, name.c_str());
    if (!lua_isfunction(luaState, -1)) {
      lua_pop(luaState, 1);
      return false;
    }

    if (lua_pcall(luaState, 0, 0, 0) != LUA_OK) {
      handleLuaError("Error calling Lua function '" + name + "': " + std::string(lua_tostring(luaState, -1)));
      return false;
    }

    return true;
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

  void LuaStateWrapper::printStack() const {
    if (!isValid()) return;

    int top = lua_gettop(luaState);
    logsManager.logMessage("Lua Stack (Top: " + std::to_string(top) + "):");

    for (int i = top; i >= 1; --i) {
      int type = lua_type(luaState, i);
      std::string typeName = lua_typename(luaState, type);
      std::string valueStr;

      switch (type) {
        case LUA_TSTRING:
          valueStr = lua_tostring(luaState, i);
          break;

        case LUA_TBOOLEAN:
          valueStr = lua_toboolean(luaState, i) ? "true" : "false";
          break;

        case LUA_TNUMBER:
          valueStr = std::to_string(lua_tonumber(luaState, i));
          break;

        case LUA_TNIL:
          valueStr = "nil";
          break;

        default:
          valueStr = typeName;
          break;
      }

      logsManager.logLuaMessage("  [" + std::to_string(i) + "] " + typeName + ": " + valueStr);
    }
  }

  void LuaStateWrapper::registerFunction(const std::string& name, lua_CFunction function) {
    if (!isValid()) return;

    lua_register(luaState, name.c_str(), function);
    if (lua_gettop(luaState) > 0 && lua_isstring(luaState, -1)) {
      const char* errorMessage = lua_tostring(luaState, -1);
      logsManager.logError("Failed to register Lua function '" + name + "': " + std::string(errorMessage));
      lua_pop(luaState, 1);
    } else {
      logsManager.logMessage("Registered Lua function: " + name);
    }
  }

  void LuaStateWrapper::handleLuaError(int errorCode) const {
    if (!luaState) return; 

    const char* errorMessage = lua_tostring(luaState, -1);
    std::string logMessage = "Lua Error [" + std::to_string(errorCode) + "]: " + (errorMessage ? std::string(errorMessage) : std::string("Unknown error"));
    luaL_traceback(luaState, luaState, errorMessage, 1);
  }

  void LuaStateWrapper::handleLuaError(const std::string& errorMessage) const {
    logsManager.logError(errorMessage);
    if (luaState && lua_gettop(luaState) > 0 && lua_isstring(luaState, -1)) {
      logsManager.logError("Lua error details: " + std::string(lua_tostring(luaState, -1)));
      lua_pop(luaState, 1);
    }
  }
}
