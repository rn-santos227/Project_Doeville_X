#include "ScriptingService.h"
#include "ScriptingCategoryResolver.h"

#include <lua.hpp>
#include <vector>
#include <unordered_map>

#include "bindings/LuaBindings.h"
#include "libraries/categories/Categories.h"
#include "libraries/constants/Constants.h"

namespace Project::Services {
  using Project::Utilities::LogsManager;
  using Project::Utilities::ConfigReader;
  using Project::Utilities::LuaStateWrapper;
  using Project::Handlers::ResourcesHandler;
  using Project::Factories::ComponentsFactory;
  using Project::Factories::EntitiesFactory;
  using Project::Factories::LayersFactory;
  using Project::Factories::GameStateFactory;
  using Project::Platform::Platform;
  using Project::Services::ScriptingCategoryResolver;
  using Project::States::GameStateManager;

  namespace Constants = Project::Libraries::Constants;
  namespace Scripts = Project::Libraries::Categories::Scripts;
  namespace fs = std::filesystem;

  ScriptingService::ScriptingService(
    SDL_Renderer* renderer,
    Platform& platform,
    LogsManager& logsManager,
    ConfigReader& configReader,
    ResourcesHandler& resourcesHandler,
    ComponentsFactory& componentsFactory,
    GameStateManager& gameStateManager)
    : renderer(renderer), 
      platform(platform),
      logsManager(logsManager),
      configReader(configReader),
      componentsFactory(componentsFactory),
      gameStateManager(gameStateManager),
      resourcesHandler(resourcesHandler),
      luaStateWrapper(logsManager),
      assetsManager(logsManager, resourcesHandler),
      assetsFactory(renderer, logsManager, resourcesHandler, assetsManager),
      entitiesFactory(logsManager, configReader, componentsFactory, gameStateManager),
      layersFactory(logsManager),
      gameStateFactory(logsManager, platform, resourcesHandler,
      gameStateManager, entitiesFactory, layersFactory) {
        componentsFactory.setAssetsManager(&assetsManager);
        Project::Bindings::LuaBindings::setAssetsManager(&assetsManager);
        Project::Bindings::LuaBindings::setAssetsFactory(&assetsFactory);
      }

  inline const std::vector<ScriptCategory> loadOrder = {
    ScriptCategory::ASSET,
    ScriptCategory::ENTITY,
    ScriptCategory::ITEM,
    ScriptCategory::LAYER,
    ScriptCategory::STATE,
    ScriptCategory::OTHER
  };

  void ScriptingService::loadScriptsFromFolder(const std::string& folderPath) {
    std::unordered_map<ScriptCategory, std::vector<std::string>> categorizedScripts;

    for (const auto& entry : fs::recursive_directory_iterator(folderPath)) {
      if (!entry.is_regular_file()) continue;

      const std::string scriptPath = entry.path().string();
      const std::string scriptName = entry.path().filename().string();

      if (scriptName.find(Constants::SCRIPT_LUA_SUFFIX) == std::string::npos) continue;

      ScriptCategory category = determineScriptType(scriptName);
      if (logsManager.checkAndLogError(category == ScriptCategory::INVALID, "Invalid script type for file: " + scriptPath)) {
        continue;
      }

      if (logsManager.checkAndLogError(!validateScript(scriptPath), "Failed to validate script: " + scriptPath)) {
        handleLuaFailure();
        return;
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
    lua_State* tempState = luaL_newstate();
    if (!tempState) {
      logsManager.logError("Failed to create temporary Lua state for validation");
      return false;
    }

    LuaStateWrapper::initializeSafeState(tempState);
    int status = luaL_loadfile(tempState, scriptPath.c_str());
    if (status != LUA_OK) {
      const char* errorMessage = lua_tostring(tempState, -1);
      logsManager.logError(
        "Lua validation error in " + scriptPath + ": " +
        (errorMessage ? std::string(errorMessage) : std::string("Unknown error")));
      lua_pop(tempState, 1);
      lua_close(tempState);
      handleLuaFailure();
      return false;
    }

    lua_pop(tempState, 1);
    lua_close(tempState);
    return true;
  }

  ScriptCategory ScriptingService::determineScriptType(const std::string& scriptName) {
    static const std::unordered_map<std::string, ScriptCategory> extensionMap = {
      {Constants::LUA_ASSET_SUFFIX, ScriptCategory::ASSET},
      {Constants::LUA_ENTITY_SUFFIX, ScriptCategory::ENTITY},
      {Constants::LUA_ITEM_SUFFIX, ScriptCategory::ITEM},
      {Constants::LUA_LAYER_SUFFIX, ScriptCategory::LAYER},
      {Constants::LUA_STATE_SUFFIX, ScriptCategory::STATE}
    };

    for (const auto& [suffix, category] : extensionMap) {
      if (scriptName.size() >= suffix.size() && scriptName.compare(scriptName.size() - suffix.size(), suffix.size(), suffix) == 0) {
        logsManager.logMessage("Script '" + scriptName + "' categorized as " + ScriptingCategoryResolver::categoryToString(category));
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
    
    if (!luaStateWrapper.loadScript(scriptPath)) {
      logsManager.flushLogs();
      return;
    }

    switch (category) {
      case ScriptCategory::ENTITY: {
        if (entitiesFactory.createEntityFromLua(scriptPath)) {
          logsManager.logMessage("Entity template loaded from " + scriptPath);
        }
        break;
      }

      case ScriptCategory::LAYER: {
        auto layer = layersFactory.createLayerFromLua(scriptPath);
        if (layer) {
          logsManager.logMessage("Layer loaded from " + scriptPath);
        }
        break;
      }

      case ScriptCategory::ASSET: {
        if (assetsFactory.createAssetFromLua(scriptPath)) {
          logsManager.logMessage("Asset loaded from " + scriptPath);
        }
        break;
      }

      case ScriptCategory::STATE: {
        if (gameStateFactory.createStateFromLua(renderer, scriptPath)) {
          logsManager.logMessage("State loaded from " + scriptPath);
        }
        break;
      }

      default: {
        logsManager.logMessage("Script loaded from " + scriptPath + " with category " + ScriptingCategoryResolver::categoryToString(category));
        break;
      }
    }
  }

  void ScriptingService::handleLuaFailure() {
    logsManager.flushLogs();
    SDL_Event quitEvent{};
    quitEvent.type = SDL_QUIT;
    SDL_PushEvent(&quitEvent);
    platform.requestExit();
  }
}
