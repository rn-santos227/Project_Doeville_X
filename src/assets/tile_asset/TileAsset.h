#ifndef TILE_ASSET_H
#define TILE_ASSET_H

#include "TileAssetData.h"

#include <string>
#include <unordered_map>
#include <SDL.h>

#include "assets/BaseAsset.h"
#include "libraries/constants/NameConstants.h"
#include "utilities/lua_state_wrapper/LuaStateWrapper.h"

namespace Project::Assets {
  class TileAsset : public BaseAsset {
  public:
    TileAsset(
      SDL_Renderer* renderer,
      Project::Utilities::LogsManager& logsManager,
      Project::Handlers::ResourcesHandler& resourcesHandler
    );
    ~TileAsset() override;

    bool loadFromLua(
      const std::string& scriptPath,
      const std::string& assetName = Project::Libraries::Constants::EMPTY_STRING
    ) override;

    SDL_Texture* getTexture() const { return data.texture; }
    SDL_Rect getTileRect(char id) const;
    bool isTilePassable(char id) const;
    
    int getTileWidth() const { return tileData.width; }
    int getTileHeight() const { return tileData.height; }

  private:
    TileAssetData tileData;
  };
}

#endif
