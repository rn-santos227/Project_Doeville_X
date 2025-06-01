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

    window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, windowFlags);
    if (logsManager.checkAndLogError(!window, "Window could not be created! SDL_Error: " + std::string(SDL_GetError()))) {
      return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
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