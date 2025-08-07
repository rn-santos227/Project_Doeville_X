#ifndef BASE_ASSET_H
#define BASE_ASSET_H

#include <string>

#include <SDL.h>

#include "handlers/resources/ResourcesHandler.h"
#include "utilities/logs_manager/LogsManager.h"

namespace Project::Assets {
  class BaseAsset {
  public:
    BaseAsset(
      SDL_Renderer* renderer,
      Project::Utilities::LogsManager& logsManager,
      Project::Handlers::ResourcesHandler& resourcesHandler
    );
    
    virtual ~BaseAsset() = default;
  };
}

#endif
