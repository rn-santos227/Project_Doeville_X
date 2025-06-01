#include "SDLManager.h"

namespace Project::Core {
  SDLManager::SDLManager(LogsManager& logsManager)
    : window(nullptr), renderer(nullptr), logsManager(logsManager), initialized(false) {}

  SDLManager::~SDLManager() {
    cleanup();
  }

  bool SDLManager::initialize(const std::string& title, int width, int height, bool fullscreen) {

  }

  void SDLManager::cleanup() {
    if (renderer) {
      SDL_DestroyRenderer(renderer);
      renderer = nullptr;
    }

    if (window)  {
      SDL_DestroyWindow(window);
      window = nullptr;  
    }

    SDL_Quit();
    initialized = false;
  }
}