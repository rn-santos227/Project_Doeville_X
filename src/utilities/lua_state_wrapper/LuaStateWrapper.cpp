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
}