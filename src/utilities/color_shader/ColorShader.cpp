#include "ColorShader.h"

namespace Project::Utilities {
  bool ColorShader::apply(SDL_Texture* texture, SDL_Color color) {
    if (!texture) {
      return false;
    }
    if (SDL_SetTextureColorMod(texture, color.r, color.g, color.b) != 0) {
      return false;
    }
    if (SDL_SetTextureAlphaMod(texture, color.a) != 0) {
      return false;
    }
    return true;
  }
}
