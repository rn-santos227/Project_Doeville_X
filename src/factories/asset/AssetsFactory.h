#ifndef ASSETS_FACTORY_H
#define ASSETS_FACTORY_H

#include <memory>
#include <string>
#include <SDL2/SDL.h>

#include "assets/AssetsManager.h"
#include "handlers/resources/ResourcesHandler.h"
#include "libraries/constants/NameConstants.h"
#include "utilities/logs_manager/LogsManager.h"

namespace Project::Factories {
  class AssetsFactory {
  public:
    explicit AssetsFactory(
      SDL_Renderer* renderer,
      Project::Utilities::LogsManager& logsManager,
      Project::Handlers::ResourcesHandler& resourcesHandler,
      Project::Assets::AssetsManager& assetsManager
    );

    bool createAssetFromLua(
      const std::string& scriptPath,
      const std::string& assetName = Project::Libraries::Constants::EMPTY_STRING
    );

  private:
    SDL_Renderer* renderer;
    Project::Utilities::LogsManager& logsManager;
    Project::Handlers::ResourcesHandler& resourcesHandler;
    Project::Assets::AssetsManager& assetsManager;
  };
}

#endif
