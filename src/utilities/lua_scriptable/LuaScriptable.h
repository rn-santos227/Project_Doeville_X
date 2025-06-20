#ifndef LUA_SCRIPTABLE_H
#define LUA_SCRIPTABLE_H

#include "utilities/lua_state_wrapper/LuaStateWrapper.h"
#include "utilities/logs_manager/LogsManager.h"

namespace Project::Utilities {
  class LuaScriptable {
  public:
    explicit LuaScriptable(LogsManager& logsManager);
    virtual ~LuaScriptable() = default;

    lua_State* getLuaState() const { return luaStateWrapper.get(); }

    virtual bool attachLuaScript(const std::string& scriptPath);
    virtual bool callLuaFunction(const std::string& functionName);

  protected:
    LogsManager& logsManager;
    LuaStateWrapper luaStateWrapper;
  };
}
#endif
