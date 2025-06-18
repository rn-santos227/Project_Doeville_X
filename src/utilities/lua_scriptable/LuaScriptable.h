#ifndef LUA_SCRIPTABLE_H
#define LUA_SCRIPTABLE_H

#include "utilities/lua_state_wrapper/LuaStateWrapper.h"
#include "utilities/logs_manager/LogsManager.h"

namespace Project::Utilities {
  class LuaScriptable {
  public:
    explicit LuaScriptable(LogsManager& logsManager);
    virtual ~LuaScriptable() = default;
  };
}
#endif
