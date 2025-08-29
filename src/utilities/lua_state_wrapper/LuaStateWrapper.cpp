#include "LuaStateWrapper.h"

#include <cmath>

#include "libraries/constants/Constants.h"
#include "libraries/keys/Keys.h"

namespace Project::Utilities {
  namespace Constants = Project::Libraries::Constants;

  static int luaFastDistance(lua_State* L) {
    double x1 = luaL_checknumber(L, Constants::INDEX_ONE);
    double y1 = luaL_checknumber(L, Constants::INDEX_TWO);
    double x2 = luaL_checknumber(L, Constants::INDEX_THREE);
    double y2 = luaL_checknumber(L, Constants::INDEX_FOUR);
    double dx = x2 - x1;
    double dy = y2 - y1;
    lua_pushnumber(L, std::sqrt(dx * dx + dy * dy));
    return 1;
  }
  
  LuaStateWrapper::LuaStateWrapper(LogsManager& logsManager)
    : luaState(luaL_newstate()),
      logsManager(logsManager),
      persistentBytecodeCache(Constants::LUA_BYTECODE_CACHE_FILE) {
    if (logsManager.checkAndLogError(!luaState, "Failed to create Lua state")) {
      return;
    }
    initializeSafeState(luaState);
    registerFunction(Project::Libraries::Keys::LUA_FUNC_FAST_DISTANCE, luaFastDistance);
  }

  LuaStateWrapper::~LuaStateWrapper() {
    if (luaState) {
      lua_close(luaState);
      luaState = nullptr;
    }
     persistentBytecodeCache.save();
  }

  void LuaStateWrapper::initializeSafeState(lua_State* state) {
    if (!state) return;

    static const struct luaL_Reg safeLibs[] = {
      {"_G", luaopen_base},
      {LUA_TABLIBNAME, luaopen_table},
      {LUA_STRLIBNAME, luaopen_string},
      {LUA_MATHLIBNAME, luaopen_math},
      {LUA_COLIBNAME, luaopen_coroutine},
      {LUA_UTF8LIBNAME, luaopen_utf8},
      {nullptr, nullptr}
    };

    for (const luaL_Reg* lib = safeLibs; lib->func; ++lib) {
      luaL_requiref(state, lib->name, lib->func, 1);
      lua_pop(state, 1);
    }

    const char* blocked[] = {"dofile", "loadfile", "load", "loadstring", "require", nullptr};
    for (const char** name = blocked; *name; ++name) {
      lua_pushnil(state);
      lua_setglobal(state, *name);
    }

    lua_getglobal(state, "os");
    if (lua_istable(state, -1)) {
      const char* osBlocked[] = {"execute", "remove", "rename", "tmpname", "exit", "setlocale", "getenv", nullptr};
      for (const char** name = osBlocked; *name; ++name) {
        lua_pushnil(state);
        lua_setfield(state, -2, *name);
      }
    }

    lua_pop(state, 1);

    lua_pushnumber(state, Constants::DEFAULT_SCREEN_CENTER_X);
    lua_setglobal(state, Project::Libraries::Keys::CENTER_X);

    lua_pushnumber(state, Constants::DEFAULT_SCREEN_CENTER_Y);
    lua_setglobal(state, Project::Libraries::Keys::CENTER_Y);
  }

  lua_State* LuaStateWrapper::get() const {
    return luaState;
  }

  void LuaStateWrapper::reset() {
    if (luaState) {
      lua_close(luaState);
    }

    luaState = luaL_newstate();
    if (!luaState) {
      logsManager.logError("Failed to recreate Lua state");
      return;
    }

    initializeSafeState(luaState);
    registeredFunctions.clear();
    compiledScriptCache.clear();
    functionRefCache.clear();
    persistentBytecodeCache.load();
  }

  bool LuaStateWrapper::isValid() const {
    return luaState != nullptr;
  }

  bool LuaStateWrapper::loadScript(const std::string& scriptPath) {
    if (logsManager.checkAndLogError(!isValid(), "Lua state is invalid. Cannot load script: " + scriptPath)) {
      return false;
    }

    functionRefCache.clear();

    auto cacheIt = compiledScriptCache.find(scriptPath);
    int status = LUA_OK;
    if (cacheIt != compiledScriptCache.end()) {
      const auto& bytecode = cacheIt->second;
      status = luaL_loadbuffer(luaState, bytecode.data(), bytecode.size(), scriptPath.c_str());
    } else {
      std::vector<char> bytecode;
      if (persistentBytecodeCache.getData(scriptPath, bytecode)) {
        compiledScriptCache[scriptPath] = bytecode;
        status = luaL_loadbuffer(luaState, bytecode.data(), bytecode.size(), scriptPath.c_str());
      } else {
        status = luaL_loadfile(luaState, scriptPath.c_str());
        if (status == LUA_OK) {
          auto writer = [](lua_State*, const void* p, size_t sz, void* ud) -> int {
            auto* buffer = static_cast<std::vector<char>*>(ud);
            const char* cp = static_cast<const char*>(p);
            buffer->insert(buffer->end(), cp, cp + sz);
            return 0;
          };

          std::vector<char> newBytecode;
          if (lua_dump(luaState, writer, &newBytecode, 0) == 0) {
            compiledScriptCache[scriptPath] = newBytecode;
            persistentBytecodeCache.setData(scriptPath, newBytecode);
          } else {
            logsManager.logError("Failed to dump bytecode for " + scriptPath);
          }
        }
      }
    }

    if (status != LUA_OK) {
      handleLuaError(status);
      lua_pop(luaState, 1);
      return false;
    }

    status = lua_pcall(luaState, 0, LUA_MULTRET, 0);
    if (status != LUA_OK) {
      handleLuaError(status);
      return false;
    }

    return true;
  }

  bool LuaStateWrapper::loadScriptFromString(const std::string& code) {
    if (logsManager.checkAndLogError(!isValid(), "Lua state is invalid. Cannot load script from string.")) {
      return false;
    }

    int status = luaL_loadstring(luaState, code.c_str());
    if (status != LUA_OK) {
      handleLuaError(status);
      lua_pop(luaState, 1);
      return false;
    }

    status = lua_pcall(luaState, 0, LUA_MULTRET, 0);
    if (status != LUA_OK) {
      handleLuaError(status);
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
    if (!lua_istable(luaState, -1)) {
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
    if (registeredFunctions.count(name)) {
      return;
    }


    lua_register(luaState, name.c_str(), function);
    if (lua_gettop(luaState) > 0 && lua_isstring(luaState, -1)) {
      const char* errorMessage = lua_tostring(luaState, -1);
      logsManager.logError("Failed to register Lua function '" + name + "': " + std::string(errorMessage));
      lua_pop(luaState, 1);
    } else {
      registeredFunctions.insert(name);
    }
  }

  void LuaStateWrapper::registerFunction(const std::string& name, lua_CFunction function, void* userData) {
    if (!isValid()) return;
    if (registeredFunctions.count(name)) {
      return;
    }

    lua_pushlightuserdata(luaState, userData);
    lua_pushcclosure(luaState, function, 1);
    lua_setglobal(luaState, name.c_str());

    if (lua_gettop(luaState) > 0 && lua_isstring(luaState, -1)) {
      const char* errorMessage = lua_tostring(luaState, -1);
      logsManager.logError("Failed to register Lua function '" + name + "': " + std::string(errorMessage));
      lua_pop(luaState, 1);
    } else {
      registeredFunctions.insert(name);
    }
  }

  bool LuaStateWrapper::callCachedFunction(const std::string& name, int nargs, int nresults) {
    if (!isValid()) return false;

    auto it = functionRefCache.find(name);
    if (it == functionRefCache.end()) {
      lua_getglobal(luaState, name.c_str());
      if (!lua_isfunction(luaState, -1)) {
        lua_pop(luaState, 1);
        return false;
      }
      int ref = luaL_ref(luaState, LUA_REGISTRYINDEX);
      it = functionRefCache.emplace(name, ref).first;
    }

    lua_rawgeti(luaState, LUA_REGISTRYINDEX, it->second);
    if (lua_pcall(luaState, nargs, nresults, 0) != LUA_OK) {
      handleLuaError(std::string("Lua function call failed: ") + std::string(lua_tostring(luaState, -1)));
      return false;
    }
    return true;
  }

  void LuaStateWrapper::handleLuaError(int errorCode) const {
    if (!luaState) return; 

    const char* errorMessage = lua_tostring(luaState, -1);
    std::string logMessage = "Lua Error [" + std::to_string(errorCode) + "]: " + (errorMessage ? std::string(errorMessage) : std::string("Unknown error"));
    
    luaL_traceback(luaState, luaState, errorMessage, 1);
    const char* traceback = lua_tostring(luaState, -1);
    if (traceback) {
      logMessage += "\n";
      logMessage += traceback;
    }
    
    logsManager.logLuaMessage(logMessage);
    logsManager.logError(logMessage);
    lua_pop(luaState, 2);
  }

  void LuaStateWrapper::handleLuaError(const std::string& errorMessage) const {
    std::string logMessage = errorMessage;

    if (luaState && lua_gettop(luaState) > 0 && lua_isstring(luaState, -1)) {
      const char* luaError = lua_tostring(luaState, -1);
      luaL_traceback(luaState, luaState, luaError, 1);
      const char* traceback = lua_tostring(luaState, -1);
      if (traceback) {
        logMessage += "\n";
        logMessage += traceback;
      }
      lua_pop(luaState, 2);
    }

    logsManager.logLuaMessage(logMessage);
    logsManager.logError(logMessage);
  }
}
