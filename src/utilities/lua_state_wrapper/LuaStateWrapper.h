#ifndef LUA_STATE_WRAPPER_H
#define LUA_STATE_WRAPPER_H

#include <lua.hpp>
#include <functional>
#include <string>
#include <optional>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include "interfaces/reset_interface/Resetable.h"
#include "utilities/binary_cache/BinaryFileCache.h"
#include "utilities/logs_manager/LogsManager.h"

namespace Project::Utilities {
  class LuaStateWrapper : public Project::Interfaces::Resetable {
  public:
    LuaStateWrapper(LogsManager& logsManager);
    ~LuaStateWrapper();

    static void initializeSafeState(lua_State* state);

    // Generic argument-based function call
    template<typename... Args>
    bool callGlobalFunctionWithArgs(const std::string& name, int nresults, Args... args) {
      if (!luaState) return false;

      lua_getglobal(luaState, name.c_str());
      if (!lua_isfunction(luaState, -1)) {
        lua_pop(luaState, 1); 
        logsManager.logError("Lua: '" + name + "' is not a function.");
        return false;
      }

      auto push = [this](auto value) {
        using T = std::decay_t<decltype(value)>;
        if constexpr (std::is_same_v<T, int>) {
          lua_pushinteger(luaState, static_cast<lua_Integer>(value));
        } else if constexpr (std::is_same_v<T, float> || std::is_same_v<T, double>) {
          lua_pushnumber(luaState, static_cast<lua_Number>(value));
        } else if constexpr (std::is_same_v<T, bool>) {
          lua_pushboolean(luaState, value);
        } else if constexpr (std::is_same_v<T, const char*>) {
          lua_pushstring(luaState, value);
        } else if constexpr (std::is_same_v<T, std::string>) {
          lua_pushstring(luaState, value.c_str());
        } else {
          logsManager.logError("Unsupported Lua argument type.");
        }
      };

      (push(args), ...);
      int argCount = sizeof...(Args);
      int result = lua_pcall(luaState, argCount, nresults, 0);
      if (result != LUA_OK) {
        handleLuaError(result);
        return false;
      }

      return true;
    }

    lua_State* get() const;
    void reset();
    
    bool isValid() const;
    bool loadScript(const std::string& scriptPath);
    bool loadScriptFromString(const std::string& code); 

    //Global Getters
    std::string getGlobalString(const std::string& name, const std::string& defaultValue = "") const;
    float getGlobalNumber(const std::string& name, float defaultValue = 0.0f) const;
    bool getGlobalBoolean(const std::string& name, bool defaultValue = false) const;

    //Global Setters
    void setGlobalString(const std::string& name, const std::string& value) const;
    void setGlobalNumber(const std::string& name, float value);
    void setGlobalBoolean(const std::string& name, bool value);

    //Table Getters
    std::string getTableString(const std::string& tableName, const std::string& key, const std::string& defaultValue = "") const;
    float getTableNumber(const std::string& tableName, const std::string& key, float defaultValue = 0.0f) const;
    bool getTableBoolean(const std::string& tableName, const std::string& key, bool defaultValue = false) const;

    // Table Setters
    void setTableString(const std::string& tableName, const std::string& key, const std::string& value);
    void setTableNumber(const std::string& tableName, const std::string& key, float value);
    void setTableBoolean(const std::string& tableName, const std::string& key, bool value);

    // Function handling
    bool isGlobalFunction(const std::string& name) const;
    bool callGlobalFunction(const std::string& name, int nargs = 0, int nresults = 0) const;
    bool callFunctionIfExists(const std::string& name);

    // Table handling
    bool isGlobalTable(const std::string& name) const;
    void iterateGlobalTable(const std::string& name, std::function<void(lua_State*, int)> callback) const;

    // Lua utilities
    static int luaPrintRedirect(lua_State* L);
    void printStack() const;
    void registerFunction(const std::string& name, lua_CFunction function);
    void registerFunction(const std::string& name, lua_CFunction function, void* userData);
    bool callCachedFunction(const std::string& name, int nargs = 0, int nresults = 0);

    // Lua error handling
    void handleLuaError(int errorCode) const;
    void handleLuaError(const std::string& errorMessage) const;

  private:
    lua_State* luaState;
    LogsManager& logsManager;
    BinaryFileCache persistentBytecodeCache;

    std::unordered_map<std::string, std::vector<char>> compiledScriptCache;
    std::unordered_map<std::string, int> functionRefCache;
    std::unordered_set<std::string> registeredFunctions;
  };
}

#endif
