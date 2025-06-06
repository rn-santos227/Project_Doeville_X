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
      luaStateWrapper(logsManager),
      gameStateManager(gameStateManager), 
      componentsFactory(componentsFactory), 
      entitiesFactory(logsManager, componentsFactory), 
      gameStateFactory(logsManager, gameStateManager) {
    logsManager.checkAndLogError(!luaStateWrapper.isValid(), "Failed to create Lua state");
    if (logsManager.checkAndLogError(!luaStateWrapper.isValid(), "Failed to create Lua state")) {
      logsManager.flushLogs();
    } else {
      lua_State* luaState = luaStateWrapper.get();
      luaL_openlibs(luaState);  
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

    for (ScriptCategory category : loadOrder) {
      const auto& scripts = categorizedScripts[category];
      for (const auto& scriptPath : scripts) {
        loadScriptByCategory(scriptPath, category);
      }
    }
  }

  bool ScriptingService::validateScript(const std::string& scriptPath) {
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
    if (logsManager.checkAndLogError(!luaStateWrapper.isValid(), "Lua state is not valid")) {
      logsManager.flushLogs();
      return;
    }
    
    int result = luaL_dofile(luaStateWrapper.get(), scriptPath.c_str());
    if (result != LUA_OK) {
      luaStateWrapper.handleLuaError(result);
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
}
