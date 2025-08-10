#include "TileHandler.h"

#include "assets/tile_asset/TileAsset.h"

namespace Project::Handlers {
  using Project::Utilities::LogsManager;
  using Project::Assets::MapAsset;
  using Project::Assets::TileAsset;
  using Project::Assets::AssetsManager;

  TileHandler::TileHandler(SDL_Renderer* renderer, LogsManager& logsManager, AssetsManager& assetsManager)
    : renderer(renderer), assetsManager(assetsManager), logsManager(logsManager) {}

  std::vector<BuiltTile> TileHandler::buildMap(const std::string& mapAssetId) {
    std::vector<BuiltTile> tilesOut;
    auto* base = assetsManager.getAsset(mapAssetId);
    auto* mapAsset = dynamic_cast<MapAsset*>(base);
    if (!mapAsset) {
      logsManager.logError("Map asset not found: " + mapAssetId);
      return tilesOut;
    }
  }
}
