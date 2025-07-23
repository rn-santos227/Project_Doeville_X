#ifndef LUA_SCRIPTABLE_H
#define LUA_SCRIPTABLE_H

#include <string>

#include "utilities/lua_state_wrapper/LuaStateWrapper.h"
#include "utilities/logs_manager/LogsManager.h"

namespace Project::Utilities {
  class LuaScriptable {
  public:
    explicit LuaScriptable(Project::Utilities::LogsManager& logsManager);
    virtual ~LuaScriptable() = default;

    lua_State* getLuaState() const { return luaStateWrapper.get(); }
    LuaStateWrapper& getLuaStateWrapper() { return luaStateWrapper; }

    Project::Utilities::LogsManager& getLogsManager() { return logsManager; }
    const Project::Utilities::LogsManager& getLogsManager() const { return logsManager; }

    virtual bool attachLuaScript(const std::string& scriptPath);
    virtual bool callLuaFunction(const std::string& functionName);
    const std::string& getScriptPath() const { return scriptPath; }
    
  protected:
    Project::Utilities::LogsManager& logsManager;
    LuaStateWrapper luaStateWrapper;
    std::string scriptPath;
  };
}

#endif
