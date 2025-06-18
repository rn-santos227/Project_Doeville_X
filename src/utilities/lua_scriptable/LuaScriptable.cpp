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

  bool LuaScriptable::attachLuaScript(const std::string& scriptPath) {
    if (logsManager.checkAndLogError(!luaStateWrapper.isValid(), "Lua state is null")) {
      return false;
    }

    if (logsManager.checkAndLogError(!luaStateWrapper.loadScript(scriptPath), "Failed to load Lua script: " + scriptPath)) {
      return false;
    }

    return true;
  }

  bool LuaScriptable::callLuaFunction(const std::string& functionName) {

  }
}
