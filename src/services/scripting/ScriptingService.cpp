#include "ScriptingService.h"

namespace fs = std::filesystem;

namespace Project::Services {
  inline const std::vector<ScriptCategory> loadOrder = {
    ScriptCategory::ENTITY,
    ScriptCategory::ITEM,
    ScriptCategory::ANIMATION,
    ScriptCategory::MAP,
    ScriptCategory::STATE,
    ScriptCategory::OTHER
  };

  ScriptingService::ScriptingService(SDL_Renderer* renderer,  LogsManager& logsManager, ComponentsFactory& componentsFactory, GameStateManager& gameStateManager)
    : renderer(renderer), 
      logsManager(logsManager), 
      componentsFactory(componentsFactory), 
      gameStateManager(gameStateManager), 
      entitiesFactory(logsManager), 
      gameStateFactory(gameStateManager, logsManager) {
    luaState = luaL_newstate();
    luaL_openlibs(luaState);
  }

  ScriptingService::~ScriptingService() {
    if (luaState) {
      lua_close(luaState);
    }
  }

  void ScriptingService::loadScriptsFromFolder(const std::string& folderPath) {
    std::unordered_map<ScriptCategory, std::vector<std::string>> categorizedScripts;

    for (const auto& entry : fs::recursive_directory_iterator(folderPath)) {
      if (!entry.is_regular_file()) continue;

      const std::string scriptPath = entry.path().string();
      const std::string scriptName = entry.path().filename().string();

      if (scriptName.find(".lua") == std::string::npos) continue;

      ScriptCategory category = determineScriptType(scriptName);
      if (logsManager.checkAndLogError(category == ScriptCategory::INVALID, "Invalid script type for file: " + scriptPath)) {
        logsManager.flushLogs();
        continue;
      }

      if (logsManager.checkAndLogError(!validateScript(scriptPath), "Failed to validate script: " + scriptPath)) {
        logsManager.flushLogs();
        continue;
      }

      categorizedScripts[category].push_back(scriptPath);
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
      case ScriptCategory::ENTITY: {
        //Entity
      }

      case ScriptCategory::STATE: {
        if(gameStateFactory.createStateFromLua(renderer, scriptPath)) {
          logsManager.logMessage("State loaded from " + scriptPath);
        }
      }
    }
  }

  void ScriptingService::handleLuaError(int errorCode) {
    const char* errorMessage = lua_tostring(luaState, -1);
    logsManager.logError("Lua Error [" + std::to_string(errorCode) + "]:" + std::string(errorMessage));
    lua_pop(luaState, 1); 
  }
}