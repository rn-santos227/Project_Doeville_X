#include "SDLManager.h"

#include <glad/glad.h>

#include "libraries/constants/Constants.h"

namespace Constants = Project::Libraries::Constants;
namespace Project::Core {
  using Project::Utilities::LogsManager;

  SDLManager::SDLManager(LogsManager& logsManager)
    : window(nullptr), renderer(nullptr), glContext(nullptr), logsManager(logsManager), initialized(false), exitRequested(false), vsyncEnabled(false) {}

  SDLManager::~SDLManager() {
    cleanup();
  }

  bool SDLManager::init(const std::string& title, int width, int height, bool fullscreen, bool vsync) {
    if (logsManager.checkAndLogError(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0,
    "SDL could not initialize! SDL_Error: " + std::string(SDL_GetError()))) {
      return false;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    Uint32 windowFlags = SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL;
    if (fullscreen) {
      windowFlags |= SDL_WINDOW_FULLSCREEN;
    }

    window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, windowFlags);
    if (logsManager.checkAndLogError(!window, "Window could not be created! SDL_Error: " + std::string(SDL_GetError()))) {
      return false;
    }
    std::string mode = fullscreen ? "Fullscreen" : "Windowed";
    logsManager.logMessage("Window and renderer validated. Mode: " + mode + ", Size: " + std::to_string(width) + "x" + std::to_string(height));

   
    Uint32 rendererFlags = SDL_RENDERER_ACCELERATED;
    vsyncEnabled = vsync;
    if (vsyncEnabled) {
      rendererFlags |= SDL_RENDERER_PRESENTVSYNC;
    }

    SDL_SetHint(SDL_HINT_RENDER_BATCHING, Constants::STR_ON);

    renderer = SDL_CreateRenderer(window, -1, rendererFlags);
    if (logsManager.checkAndLogError(!renderer, "Renderer could not be created! SDL_Error: " + std::string(SDL_GetError()))) {
      return false;
    }
    logsManager.logMessage("Renderer created successfully.");

    glContext = SDL_GL_GetCurrentContext();
    if (logsManager.checkAndLogError(!glContext, "OpenGL context could not be obtained! SDL_Error: " + std::string(SDL_GetError()))) {
      return false;
    }

    if (!gladLoadGLLoader((SDL_GL_GetProcAddress))) {
      logsManager.logError("Failed to initialize GLAD.");
      return false;
    }

    SDL_GL_SetSwapInterval(vsync ? 1 : 0);

    initialized = true;
    return true;
  }

  void SDLManager::clear() {
    if (glContext) {
      glClearColor(
        Constants::DEFAULT_BACKGROUND_COLOR.r / 255.0f,
        Constants::DEFAULT_BACKGROUND_COLOR.g / 255.0f,
        Constants::DEFAULT_BACKGROUND_COLOR.b / 255.0f,
        Constants::DEFAULT_BACKGROUND_COLOR.a / 255.0f
      );
      glClear(GL_COLOR_BUFFER_BIT);
    }

    if (renderer) {
      SDL_SetRenderDrawColor(renderer,
        Constants::DEFAULT_BACKGROUND_COLOR.r,
        Constants::DEFAULT_BACKGROUND_COLOR.g,
        Constants::DEFAULT_BACKGROUND_COLOR.b,
        Constants::DEFAULT_BACKGROUND_COLOR.a
      );
      SDL_RenderClear(renderer);
    } else {
      logsManager.logError("Attempted to clear but renderer is null.");
    }

    if (glContext) {
      SDL_GL_SwapWindow(window);
    }
  }

  void SDLManager::present() {
    if (renderer) {
      SDL_RenderPresent(renderer);
    } else {
      logsManager.logError("Attempted to present but renderer is null.");
    }
  }

  SDL_Renderer* SDLManager::getRenderer() const {
    return renderer;
  }

  SDL_Window* SDLManager::getWindow() const {
    return window;
  }

  void SDLManager::requestExit() {
    exitRequested = true;
  }

  bool SDLManager::isExitRequested() const {
    return exitRequested;
  }

  void SDLManager::clearExitRequest() {
    exitRequested = false;
  }

  void SDLManager::cleanup() {
    if (renderer) {
      SDL_DestroyRenderer(renderer);
      renderer = nullptr;
    }

    if (glContext) {
      SDL_GL_DeleteContext(glContext);
      glContext = nullptr;
    }

    if (window)  {
      SDL_DestroyWindow(window);
      window = nullptr;
    }

    SDL_Quit();
    initialized = false;
  }
}
