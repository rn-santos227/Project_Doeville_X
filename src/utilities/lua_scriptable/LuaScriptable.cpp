#include "LuaScriptable.h"

namespace Project::Utilities {
  LuaScriptable::LuaScriptable(LogsManager& logsManager)
    : logsManager(logsManager), luaStateWrapper(logsManager) {
    if (luaStateWrapper.isValid()) {

    } else {
      logsManager.logError("Failed to create Lua state");
    }
  }
}
