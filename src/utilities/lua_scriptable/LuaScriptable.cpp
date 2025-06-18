#include "LuaScriptable.h"

namespace Project::Utilities {
  LuaScriptable::LuaScriptable(LogsManager& logsManager)
    : logsManager(logsManager), luaStateWrapper(logsManager) {
    if (luaStateWrapper.isValid()) {
      luaL_openlibs(luaStateWrapper.get());
      luaStateWrapper.registerFunction("print", LuaStateWrapper::luaPrintRedirect);
    } else {
      logsManager.logError("Failed to create Lua state");
    }
  }
}
