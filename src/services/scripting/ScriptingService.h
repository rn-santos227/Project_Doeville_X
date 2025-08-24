#ifndef SCRIPTING_SERVICE_H
#define SCRIPTING_SERVICE_H

#include "ScriptingCategory.h"

#include <iostream>
#include <regex>
#include <filesystem>
#include <fstream>
#include <string>

#include <SDL2/SDL.h>

#include "assets/AssetsManager.h"
#include "factories/asset/AssetsFactory.h"
#include "factories/component/ComponentsFactory.h"
#include "factories/entity/EntitiesFactory.h"
#include "factories/layer/LayersFactory.h"
#include "factories/state/GameStateFactory.h"
#include "handlers/resources/ResourcesHandler.h"
#include "platform/Platform.h"
#include "states/GameStateManager.h"
#include "utilities/logs_manager/LogsManager.h"
#include "utilities/lua_state_wrapper/LuaStateWrapper.h"

namespace Project::Services {

  class ScriptingService {
  public:
    explicit ScriptingService(
      SDL_Renderer* renderer,
      Project::Platform::Platform& platform,
      Project::Utilities::LogsManager& logsManager,
      Project::Utilities::ConfigReader& configReader,
      Project::Handlers::ResourcesHandler& resourcesHandler,
      Project::Factories::ComponentsFactory& componentsFactory,
      Project::States::GameStateManager& gameStateManager
    );
    
    ~ScriptingService() = default;

    Project::Assets::AssetsManager& getAssetsManager() { return assetsManager; }
    Project::Factories::AssetsFactory& getAssetsFactory() { return assetsFactory; }

    bool validateScript(const std::string& scriptPath);
    void loadScriptsFromFolder(const std::string& folderPath);

  private:
    Project::Platform::Platform& platform;
    Project::Utilities::LogsManager& logsManager;
    Project::Utilities::ConfigReader& configReader;
    Project::Factories::ComponentsFactory& componentsFactory;
    Project::States::GameStateManager& gameStateManager;
    Project::Handlers::ResourcesHandler& resourcesHandler;

    Project::Assets::AssetsManager assetsManager;
    Project::Utilities::LuaStateWrapper luaStateWrapper;
    
    SDL_Renderer* renderer;

    Project::Factories::EntitiesFactory entitiesFactory;
    Project::Factories::LayersFactory layersFactory;
    Project::Factories::GameStateFactory gameStateFactory;
    Project::Factories::AssetsFactory assetsFactory;

    ScriptCategory determineScriptType(const std::string& scriptName);
    void loadScriptByCategory(const std::string& scriptPath, ScriptCategory category);
    void handleLuaFailure();
  };
}

#endif
