#ifndef ASSET_DATA_H
#define ASSET_DATA_H

#include <string>
#include <SDL.h>

#include "AssetCategory.h"

namespace Project::Assets {
  struct BaseAssetData {
    SDL_Texture* texture;
    AssetCategory category = AssetCategory::TEXTURE;

    std::string name;
    std::string path;
    std::string tag;
  };
}

#endif
