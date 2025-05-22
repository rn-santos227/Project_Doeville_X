#ifndef LUA_STATE_WRAPPER_H
#define LUA_STATE_WRAPPER_H

#include <lua.hpp>
#include <functional>
#include <string>
#include <optional>

#include "utilities/logs_manager/LogsManager.h"
namespace Project::Utilities {
  class LuaStateWrapper {
  public:
    LuaStateWrapper(LogsManager& logsManager);
    ~LuaStateWrapper();

    lua_State* get() const;
    
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

    bool isGlobalFunction(const std::string& name) const;
    bool callGlobalFunction(const std::string& name, int nargs = 0, int nresults = 0) const;
    bool callFunctionIfExists(const std::string& name);

    bool isGlobalTable(const std::string& name) const;
    void iterateGlobalTable(const std::string& name, std::function<void(lua_State*, int)> callback) const;

    // Lua utilities
    static int luaPrintRedirect(lua_State* L);

    // Lua error handling
    void handleLuaError(int errorCode) const;
    void handleLuaError(const std::string& errorMessage) const;


  private:
    lua_State* luaState;
    LogsManager& logsManager;
  };
}
#endif