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
    if (logsManager.checkAndLogError(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0, "SDL could not initialize! SDL_Error: " + std::string(SDL_GetError()))) {
      return false;
    }

    Uint32 windowFlags = SDL_WINDOW_SHOWN;
    if (fullscreen) {
      windowFlags |= SDL_WINDOW_FULLSCREEN;
    }

    openGLMode = opengl;
    if (openGLMode) {
      windowFlags |= SDL_WINDOW_OPENGL;
    }

    window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, windowFlags);
    if (logsManager.checkAndLogError(!window, "Window could not be created! SDL_Error: " + std::string(SDL_GetError()))) {
      return false;
    }

    std::string mode = fullscreen ? Constants::FULLSCREEN : Constants::WINDOWED;
    logsManager.logMessage("Window and renderer validated. Mode: " + mode + ", Size: " + std::to_string(width) + "x" + std::to_string(height));

    Uint32 rendererFlags = SDL_RENDERER_ACCELERATED;
    vsyncEnabled = vsync;
    if (vsyncEnabled) {
      rendererFlags |= SDL_RENDERER_PRESENTVSYNC;
    }

    if (!openGLMode) {
      renderer = SDL_CreateRenderer(window, -1, rendererFlags);
      if (logsManager.checkAndLogError(!renderer, "Renderer could not be created! SDL_Error: " + std::string(SDL_GetError()))) {
        return false;
      }
    } else {
      glContext = SDL_GL_CreateContext(window);
      if (logsManager.checkAndLogError(!glContext, "OpenGL context could not be created! SDL_Error: " + std::string(SDL_GetError()))) {
        return false;
      }
      SDL_GL_SetSwapInterval(vsyncEnabled ? 1 : 0);
    }

    if (rendererAPI) {
      rendererAPI->init();
    }

    logsManager.logMessage("Renderer created successfully.");
    initialized = true;
    return true;
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
