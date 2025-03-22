#include "Animation.h"
#include <SDL_image.h>

namespace Project::Handlers {
  Animation::Animation(SDL_Renderer* renderer, LogsManager& logsManager) : renderer(renderer), logsManager(logsManager) {}

  bool Animation::addFrame(const std::string& imagePath, Uint32 duration) {
    SDL_Surface* surface = IMG_Load(imagePath.c_str());

    if (logsManager.checkAndLogError(!surface, "Failed to load image: " + imagePath + " - " + IMG_GetError())) {
      logsManager.flushLogs();
      return false;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    if (logsManager.checkAndLogError(!texture, "Failed to load texture: " + imagePath + " - " + IMG_GetError())) {
      logsManager.flushLogs();
      return false;
    }
  }

  void Animation::freeFrames() {
    for (auto& frame : frames) {
      SDL_DestroyTexture(frame.texture);
    }
    frames.clear();
  }
}