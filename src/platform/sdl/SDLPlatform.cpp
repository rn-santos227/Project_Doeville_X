#include "SDLPlatform.h"

#include "libraries/constants/Constants.h"

namespace Project::Platform {
  using Project::Utilities::LogsManager;
  SDLPlatform::SDLPlatform(LogsManager& logsManager)
    : logsManager(logsManager), rendererAPI(nullptr), 
      window(nullptr), renderer(nullptr), glContext(nullptr), 
      exitRequested(false), initialized(false),
      openGLMode(false), vsyncEnabled(false) {}

  SDLPlatform::~SDLPlatform() {
    cleanup();
  }

  void SDLPlatform::requestExit() {
    exitRequested = true;
  }

  bool SDLPlatform::isExitRequested() const {
    return exitRequested;
  }

  void SDLPlatform::clearExitRequest() {
    exitRequested = false;
  }

  void SDLPlatform::cleanup() {
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

  void SDLPlatform::setRendererAPI(std::unique_ptr<RendererAPI> api) {
    rendererAPI = std::move(api);
  }
}
