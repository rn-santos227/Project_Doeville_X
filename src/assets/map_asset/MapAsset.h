#ifndef MAP_ASSET_H
#define MAP_ASSET_H

#include "MapAssetData.h"

#include <string>
#include <vector>
#include <unordered_map>
#include <SDL.h>

#include "assets/BaseAsset.h"
#include "libraries/constants/NameConstants.h"


namespace Project::Assets {
  class MapAsset : public BaseAsset {
  public:
    MapAsset(
      SDL_Renderer* renderer,
      Project::Utilities::LogsManager& logsManager,
      Project::Handlers::ResourcesHandler& resourcesHandler
    );
    ~MapAsset() override = default;

    bool loadFromLua(
      const std::string& scriptPath,
      const std::string& assetName = Project::Libraries::Constants::EMPTY_STRING
    ) override;

    const std::vector<std::vector<int>>& getMap() const { return mapData.layout; }
    const std::unordered_map<int, std::string>& getTileMappings() const { return mapData.tiles; }
    
    int getWidth() const { return mapData.cols; }
    int getHeight() const { return mapData.rows; }

  private:
    MapAssetData mapData;
  };
}

#endif
