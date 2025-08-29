#include "LuaScriptable.h"

#include "libraries/keys/Keys.h"

namespace Project::Utilities {
  namespace Keys = Project::Libraries::Keys;

  LuaScriptable::LuaScriptable(LogsManager& logsManager)
    : logsManager(logsManager), luaStateWrapper(logsManager) {
    if (luaStateWrapper.isValid()) {
      luaStateWrapper.registerFunction(Keys::LUA_FUNC_PRINT, LuaStateWrapper::luaPrintRedirect);
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

    this->scriptPath = scriptPath;
    return true;
  }

  bool LuaScriptable::callLuaFunction(const std::string& functionName) {
    return luaStateWrapper.callCachedFunction(functionName);
  }
}
