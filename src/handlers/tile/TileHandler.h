#ifndef TILE_HANDLER_H
#define TILE_HANDLER_H

#include <string>
#include <vector>
#include <SDL.h>

#include "assets/AssetsManager.h"
#include "assets/map_asset/MapAsset.h"
#include "utilities/logs_manager/LogsManager.h"

namespace Project::Handlers {
  struct BuiltTile {
    SDL_Texture* texture;
    SDL_Rect src;
    SDL_Rect dest;
    bool passable;
  };

  class TileHandler {
  public:
    TileHandler(SDL_Renderer* renderer, Project::Utilities::LogsManager& logsManager, Project::Assets::AssetsManager& assetsManager);
    ~TileHandler() = default;

    std::vector<BuiltTile> buildMap(const std::string& mapAssetId);
 
  private:
    SDL_Renderer* renderer;
    Project::Assets::AssetsManager& assetsManager;
    Project::Utilities::LogsManager& logsManager;
  };
}

#endif
