#include "ScriptingService.h"

#include <vector>
#include <unordered_map>

#include "libraries/constants/Constants.h"

namespace Constants = Project::Libraries::Constants;
namespace fs = std::filesystem;

namespace Project::Services {
  using Project::Utilities::LogsManager;
  using Project::Utilities::LuaStateWrapper;
  using Project::Handlers::ResourcesHandler;
  using Project::Factories::ComponentsFactory;
  using Project::States::GameStateManager;
  using Project::Factories::EntitiesFactory;
  using Project::Factories::GameStateFactory;

  inline const std::vector<ScriptCategory> loadOrder = {
    ScriptCategory::ENTITY,
    ScriptCategory::ITEM,
    ScriptCategory::ANIMATION,
    ScriptCategory::MAP,
    ScriptCategory::STATE,
    ScriptCategory::OTHER
  };

  static std::string categoryToString(ScriptCategory category) {
    switch (category) {
      case ScriptCategory::ENTITY: return "ENTITY";
      case ScriptCategory::ITEM: return "ITEM";
      case ScriptCategory::ANIMATION: return "ANIMATION";
      case ScriptCategory::MAP: return "MAP";
      case ScriptCategory::STATE: return "STATE";
      case ScriptCategory::OTHER: return "OTHER";
      default: return "INVALID";
    }
  }

  ScriptingService::ScriptingService(SDL_Renderer* renderer,  LogsManager& logsManager, ResourcesHandler& resourcesHandler, ComponentsFactory& componentsFactory, GameStateManager& gameStateManager)
    : renderer(renderer), 
      logsManager(logsManager), 
      luaStateWrapper(logsManager),
      resourcesHandler(resourcesHandler),
      gameStateManager(gameStateManager), 
      componentsFactory(componentsFactory), 
      entitiesFactory(logsManager, componentsFactory),
      gameStateFactory(logsManager, resourcesHandler, gameStateManager, entitiesFactory) {
    if(logsManager.checkAndLogError(!luaStateWrapper.isValid(), "Failed to create Lua state")) {
      return;
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
        continue;
      }

      if (logsManager.checkAndLogError(!validateScript(scriptPath), "Failed to validate script: " + scriptPath)) {
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
      {Constants::LUA_STATE_SUFFIX, ScriptCategory::STATE},
      {Constants::LUA_ENTITY_SUFFIX, ScriptCategory::ENTITY},
      {Constants::LUA_MAP_SUFFIX, ScriptCategory::MAP},
      {Constants::LUA_ITEM_SUFFIX, ScriptCategory::ITEM},
      {Constants::LUA_ANIMATION_SUFFIX, ScriptCategory::ANIMATION}
    };

    for (const auto& [suffix, category] : extensionMap) {
      if (scriptName.size() >= suffix.size() && scriptName.compare(scriptName.size() - suffix.size(), suffix.size(), suffix) == 0) {
        logsManager.logMessage("Script '" + scriptName + "' categorized as " + categoryToString(category));
        return category;
      }
    }

    logsManager.logWarning("Unable to determine script type for " + scriptName);
    return ScriptCategory::INVALID;
  }

  void ScriptingService::loadScriptByCategory(const std::string& scriptPath, ScriptCategory category) {
    if (logsManager.checkAndLogError(!luaStateWrapper.isValid(), "Lua state is not valid")) {
      return;
    }
    
    int result = luaL_dofile(luaStateWrapper.get(), scriptPath.c_str());
    if (result != LUA_OK) {
      luaStateWrapper.handleLuaError(result);
      return;
    }

    switch (category) {
      case ScriptCategory::ENTITY: {
        if (entitiesFactory.createEntityFromLua(scriptPath)) {
          logsManager.logMessage("Entity template loaded from " + scriptPath);
        }
        break;
      }

      case ScriptCategory::STATE: {
        if(gameStateFactory.createStateFromLua(renderer, scriptPath)) {
          logsManager.logMessage("State loaded from " + scriptPath);
        }
        break;
      }

      default: {
        logsManager.logMessage("Script loaded from " + scriptPath + " with category " + categoryToString(category));
        break; 
      }
    }
  }
}
