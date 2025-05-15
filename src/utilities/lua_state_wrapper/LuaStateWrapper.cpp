#include "LuaStateWrapper.h"

namespace Project::Utilities {
  LuaStateWrapper::LuaStateWrapper(LogsManager& logsManager) : luaState(luaL_newstate()), logsManager(logsManager) {
    if (luaState) {
      luaL_openlibs(luaState);
    }
  }
}