#ifndef TILE_ASSET_H
#define TILE_ASSET_H

#include "TileAssetData.h"

#include <string>
#include <unordered_map>
#include <SDL.h>

#include "assets/BaseAsset.h"
#include "utilities/lua_state_wrapper/LuaStateWrapper.h"

namespace Project::Assets {
  class TileAsset : public BaseAsset {
  public:

  private:
    TileAssetData tileData;
  };
}

#endif
