#ifndef TEXTURE_HOLDER_H
#define TEXTURE_HOLDER_H

#include <SDL.h>

namespace Project::Components {
  class TextureHolder {
  protected:
    SDL_Texture* texture;

    void destroyTexture() {
      if (texture) {
        SDL_DestroyTexture(texture);
        texture = nullptr;
      }
    }
  };
}
#endif
