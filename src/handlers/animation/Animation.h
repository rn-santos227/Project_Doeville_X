#ifndef ANIMATION_H
#define ANIMATION_H

#include "PlaybackMode.h"

#include <vector>
#include <string>
#include <SDL.h>

namespace Project::Handlers {
  struct AnimationFrame {
    SDL_Texture* texture;
    Uint32 duration;
  };

  class Animation {

  };
}

#endif