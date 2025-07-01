#ifndef COLOR_UTILS_H
#define COLOR_UTILS_H

#include <SDL.h>
#include <string>

namespace Project::Utilities {
  SDL_Color hexToRGB(const std::string& hex, Uint8 alpha = 255);
}

#endif
