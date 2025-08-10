#include "TileHandler.h"

#include "assets/tile_asset/TileAsset.h"

namespace Project::Handlers {
  using Project::Utilities::LogsManager;
  using Project::Assets::TileAsset;
  using Project::Assets::TileAsset;
  using Project::Assets::AssetsManager;

  TileHandler::TileHandler(SDL_Renderer* renderer, LogsManager& logsManager, AssetsManager& assetsManager)
    : renderer(renderer), assetsManager(assetsManager), logsManager(logsManager) {}
}
