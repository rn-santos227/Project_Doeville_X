#include "SDLPlatform.h"

#include "libraries/constants/Constants.h"

namespace Project::Platform {
  using Project::Utilities::LogsManager;

  namespace Constants = Project::Libraries::Constants;
  SDLPlatform::SDLPlatform(LogsManager& logsManager)
    : logsManager(logsManager), rendererAPI(nullptr), 
      window(nullptr), renderer(nullptr), glContext(nullptr), 
      exitRequested(false), initialized(false),
      openGLMode(false), vsyncEnabled(false) {}

  SDLPlatform::~SDLPlatform() {
    cleanup();
  }

  SDL_Renderer* SDLPlatform::getRenderer() const {
    return renderer;
  }

  SDL_Window* SDLPlatform::getWindow() const {
    return window;
  }

  bool SDLPlatform::init(const std::string& title, int width, int height, bool fullscreen, bool vsync, bool opengl) {

  }

  void SDLPlatform::present() {
    if (rendererAPI) {
      rendererAPI->present();
      return;
    }
    if (renderer) {
      SDL_RenderPresent(renderer);
    } else {
      logsManager.logError("Attempted to present but renderer is null.");
    }
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

  void SDLPlatform::clear() {
    if (rendererAPI) {
      rendererAPI->clear();
      return;
    }

    if (renderer) {
      SDL_SetRenderDrawColor(renderer,
        Constants::DEFAULT_BACKGROUND_COLOR.r,
        Constants::DEFAULT_BACKGROUND_COLOR.g,
        Constants::DEFAULT_BACKGROUND_COLOR.b,
        Constants::DEFAULT_BACKGROUND_COLOR.a);
      SDL_RenderClear(renderer);
    } else {
      logsManager.logError("Attempted to clear but renderer is null.");
    }
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

  void SDLPlatform::setRendererAPI(std::unique_ptr<RendererAPI> api) {
    rendererAPI = std::move(api);
  }
}
