#include "SDLManager.h"

namespace Project::Core {
  SDLManager::SDLManager(LogsManager& logsManager)
    : window(nullptr), renderer(nullptr), logsManager(logsManager), initialized(false) {}

  SDLManager::~SDLManager() {
    cleanup();
  }

  bool SDLManager::initialize(const std::string& title, int width, int height, bool fullscreen) {
    if (logsManager.checkAndLogError(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0,
    "SDL could not initialize! SDL_Error: " + std::string(SDL_GetError()))) {
      return false;
    }

    Uint32 windowFlags = SDL_WINDOW_SHOWN;
    if (fullscreen) {
      windowFlags |= SDL_WINDOW_FULLSCREEN;
    }
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