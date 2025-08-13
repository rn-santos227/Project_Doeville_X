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
    if (!surface) return nullptr;
    int width = surface->w;
    int height = surface->h;
    SDL_Surface* current = surface;
    while ((width > maxDimension || height > maxDimension) && width > 1 && height > 1) {
      int newW = width / Project::Libraries::Constants::INDEX_TWO;
      int newH = height / Project::Libraries::Constants::INDEX_TWO;
      SDL_Surface* scaled = SDL_CreateRGBSurfaceWithFormat(0, newW, newH, surface->format->BitsPerPixel, surface->format->format);
      if (!scaled) break;
      SDL_Rect dst{0, 0, newW, newH};
      SDL_BlitScaled(current, nullptr, scaled, &dst);
      if (current != surface) SDL_FreeSurface(current);
      current = scaled;
      width = newW;
      height = newH;
    }
    return current;
  }
}
