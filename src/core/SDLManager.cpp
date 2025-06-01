#include "SDLManager.h"

namespace Project::Core {
  SDLManager::SDLManager(LogsManager& logsManager)
    : window(nullptr), renderer(nullptr), logsManager(logsManager), initialized(false) {}

  void SDLManager::cleanup() {
    if (renderer) {
      SDL_DestroyRenderer(renderer);
      renderer = nullptr;
    }
  }
}