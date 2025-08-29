#ifndef TEXTURE_UTILS_H
#define TEXTURE_UTILS_H

#include <SDL.h>

namespace Project::Utilities::TextureUtils {
  SDL_Surface* compress(SDL_Surface* surface);
  SDL_Surface* selectMip(SDL_Surface* surface, int maxDimension);
}

#endif
