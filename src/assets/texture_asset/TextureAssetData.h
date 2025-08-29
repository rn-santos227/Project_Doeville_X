#ifndef TEXTURE_ASSET_DATA_H
#define TEXTURE_ASSET_DATA_H

#include <optional>
#include <SDL.h>

#include "libraries/constants/ColorConstants.h"
#include "libraries/constants/FloatConstants.h"

namespace Project::Assets {
  struct TextureAssetData {
    std::optional<SDL_Color> color;
    float scale = Project::Libraries::Constants::DEFAULT_WHOLE;
    int width{0};
    int height{0};
  };
}

#endif
