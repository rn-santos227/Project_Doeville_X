#ifndef TEXTURE_ASSET_DATA_H
#define TEXTURE_ASSET_DATA_H

#include <SDL.h>

#include "libraries/constants/ColorConstants.h"
#include "libraries/constants/FloatConstants.h"

namespace Project::Assets {
  struct TextureAssetData {
    SDL_Color color = Project::Libraries::Constants::COLOR_WHITE;

    float scale = Project::Libraries::Constants::DEFAULT_WHOLE;

    int width{0};
    int height{0};
  };
}

#endif
