#include "Animation.h"
#include <SDL_image.h>

namespace Project::Handlers {
  Animation::Animation(SDL_Renderer* renderer, LogsManager& logsManager) : renderer(renderer), logsManager(logsManager) {}

  bool Animation::addFrame(const std::string& imagePath, Uint32 duration) {
    SDL_Surface* surface = IMG_Load(imagePath.c_str());
  }

  void Animation::freeFrames() {
    for (auto& frame : frames) {
      SDL_DestroyTexture(frame.texture);
    }
    frames.clear();
  }
}