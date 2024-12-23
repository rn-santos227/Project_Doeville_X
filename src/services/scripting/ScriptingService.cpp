#include "ScriptingService.h"

namespace fs = std::filesystem;

namespace Project::Services {
  ScriptingService::ScriptingService(GameStateManager& gameStateManager, LogsManager& logsManager) : 
    gameStateManager(gameStateManager), logsManager(logsManager), gameStateFactory(gameStateManager, logsManager) {
    luaState = luaL_newstate();
    luaL_openlibs(luaState);
  }

  ScriptingService::~ScriptingService() {
    if (luaState) {
      lua_close(luaState);
    }
  }

  void ScriptingService::loadScriptsFromFolder(const std::string& folderPath) {
    for (const auto& entry : fs::recursive_directory_iterator(folderPath)) {
      if (entry.is_regular_file()) {
        const std::string scriptPath  = entry.path().string();
        const std::string scriptName  = entry.path().filename().string();

        if (fs::is_directory(entry) || scriptName.find(".lua") == std::string::npos) {
          continue;
        }

        ScriptCategory category = determineScriptType(scriptName);
        if (category != ScriptCategory::INVALID) {
          validateScript(scriptPath);
        } else {
          logsManager.logError("Invalid script naming: " + scriptName);
        }
      }
    }
  }
  
  bool ScriptingService::validateScript(const std::string& scriptPath) {
    int result = luaL_loadfile(luaState, scriptPath.c_str());
    if (result != LUA_OK) {
      handleLuaError(result);
      return false;
    }
    lua_pop(luaState, 1);
    return true;
  }

  ScriptCategory ScriptingService::determineScriptType(const std::string& scriptName) {
    static const std::unordered_map<std::string, ScriptCategory> extensionMap = {
      {".state.lua", ScriptCategory::STATE},
      {".entity.lua", ScriptCategory::ENTITY},
      {".map.lua", ScriptCategory::MAP},
      {".item.lua", ScriptCategory::ITEM},
      {".animation.lua", ScriptCategory::ANIMATION}
    };

    for (const auto& [suffix, category] : extensionMap) {
      if (scriptName.size() >= suffix.size() &&
        scriptName.compare(scriptName.size() - suffix.size(), suffix.size(), suffix) == 0) {
        return category;
      }
    }

    return ScriptCategory::INVALID;
  }

  void ScriptingService::loadScriptByCategory(const std::string& scriptPath, ScriptCategory category) {
    int result = luaL_dofile(luaState, scriptPath.c_str());
    if (result != LUA_OK) {
      handleLuaError(result);
      return;
    }

    switch (category) {
      case ScriptCategory::STATE: {
        if(gameStateFactory.createStateFromLua(scriptPath)) {
          logsManager.logMessage("State loaded from " + scriptPath);
        }
        break;
      }
    }
  }

  void ScriptingService::handleLuaError(int errorCode) {
    const char* errorMessage = lua_tostring(luaState, -1);
    logsManager.logError("Lua Error [" + std::to_string(errorCode) + "]:" + std::string(errorMessage));
    lua_pop(luaState, 1); 
  }
}