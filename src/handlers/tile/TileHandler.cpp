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
    MapAsset* mapAsset = assetsManager.getMap(mapAssetId);
    if (!mapAsset) {
      logsManager.logError("Map asset not found: " + mapAssetId);
      return tilesOut;
    }

    const auto& layout = mapAsset->getMap();
    const auto& mapping = mapAsset->getTileMappings();
    int tileWidth = 0;
    int tileHeight = 0;

    for (size_t row = 0; row < layout.size(); ++row) {
      const auto& line = layout[row];
      for (size_t col = 0; col < line.size(); ++col) {
        int id = line[col];
        auto it = mapping.find(id);
        if (it == mapping.end()) continue;

        TileAsset* tileAsset = assetsManager.getTile(it->second);
        if (!tileAsset) continue;
        if (tileWidth == 0 || tileHeight == 0) {
          tileWidth = tileAsset->getTileWidth();
          tileHeight = tileAsset->getTileHeight();
        }

        SDL_Rect src = tileAsset->getTileRect(static_cast<char>(id));
        SDL_Rect dest{static_cast<int>(col * tileWidth), static_cast<int>(row * tileHeight), tileWidth, tileHeight};
        tilesOut.push_back({tileAsset->getTexture(), src, dest, tileAsset->isTilePassable(static_cast<char>(id))});
      }
    }

    return tilesOut;
  }
}
