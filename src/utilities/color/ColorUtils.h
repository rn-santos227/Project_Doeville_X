#ifndef COLOR_UTILS_H
#define COLOR_UTILS_H

#include <SDL.h>
#include <string>

#include "libraries/constants/Constants.h"

namespace Project::Utilities {
  class ColorUtils {
  public:
    static SDL_Color hexToRGB(const std::string& hex, Uint8 alpha = Project::Libraries::Constants::FULL_ALPHA);
    static SDL_Color negative(SDL_Color color);
    static bool applyShader(SDL_Texture* texture, SDL_Color color);
  };
}

#endif
