#ifndef COLOR_SHADER_H
#define COLOR_SHADER_H

#include <SDL.h>

namespace Project::Utilities {
  class ColorShader {
  public:
    static bool apply(SDL_Texture* texture, SDL_Color color);
  };
}

#endif
