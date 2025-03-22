#include "Animation.h"
#include <SDL_image.h>

namespace Project::Handlers {
  Animation::Animation(SDL_Renderer* renderer) : renderer(renderer) {}

  bool Animation::addFrame(const std::string& imagePath, Uint32 duration) {

  }

  void Animation::freeFrames() {
    for (auto& frame : frames) {
      SDL_DestroyTexture(frame.texture);
    }
    frames.clear();
  }
}