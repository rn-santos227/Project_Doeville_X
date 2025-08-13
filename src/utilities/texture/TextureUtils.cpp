#include "TextureUtils.h"

#include "libraries/constants/IndexConstants.h"

namespace Project::Utilities::TextureUtils {
  SDL_Surface* compress(SDL_Surface* surface) {
    if (!surface) return nullptr;
    SDL_Surface* converted = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_RGBA32, 0);
    if (converted && converted != surface) {
      SDL_FreeSurface(surface);
      surface = converted;
    }
    return surface;
  }

  SDL_Surface* selectMip(SDL_Surface* surface, int maxDimension) {

  }
}
