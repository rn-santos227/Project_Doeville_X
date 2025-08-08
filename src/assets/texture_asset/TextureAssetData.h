#ifndef TEXTURE_ASSET_DATA_H
#define TEXTURE_ASSET_DATA_H

#include "libraries/constants/FloatConstants.h"

namespace Project::Assets {
  struct TextureAssetData {
    int width{0};
    int height{0};
    float scale = Project::Libraries::Constants::DEFAULT_WHOLE;
  };
}

#endif
