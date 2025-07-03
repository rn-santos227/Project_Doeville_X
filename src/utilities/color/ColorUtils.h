#ifndef COLOR_UTILS_H
#define COLOR_UTILS_H

#include <SDL.h>
#include <string>

#include "libraries/constants/Constants.h"

namespace Constants = Project::Libraries::Constants;

namespace Project::Utilities {
  class ColorUtils {
  public:
    static SDL_Color hexToRGB(const std::string& hex, Uint8 alpha = Constants::FULL_ALPHA);
  };
}

#endif
