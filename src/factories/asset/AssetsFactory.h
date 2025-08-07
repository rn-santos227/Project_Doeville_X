#ifndef ASSETS_FACTORY_H
#define ASSETS_FACTORY_H

#include <memory>
#include <string>
#include <SDL2/SDL.h>

#include "assets/AssetsManager.h"
#include "handlers/resources/ResourcesHandler.h"
#include "utilities/logs_manager/LogsManager.h"

namespace Project::Factories {
  class AssetsFactory {
  public:

  private:
    SDL_Renderer* renderer;
    Project::Utilities::LogsManager& logsManager;
    Project::Handlers::ResourcesHandler& resourcesHandler;
    Project::Assets::AssetsManager& assetsManager;
  };
}

#endif
