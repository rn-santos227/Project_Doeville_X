#include "Animation.h"
#include <SDL_image.h>

namespace Project::Handlers {
  Animation::Animation(SDL_Renderer* renderer) : renderer(renderer) {}

  void Animation::freeFrames() {
    for (auto& frame : frames) {

    }
  }
}