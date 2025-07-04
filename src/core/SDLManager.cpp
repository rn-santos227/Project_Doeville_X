#include "SDLManager.h"

#include "libraries/constants/Constants.h"

namespace Constants = Project::Libraries::Constants;
namespace Project::Core {
  using Project::Utilities::LogsManager;

  SDLManager::SDLManager(LogsManager& logsManager)
    : window(nullptr), renderer(nullptr), logsManager(logsManager), initialized(false), exitRequested(false) {}

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
    std::string mode = fullscreen ? "Fullscreen" : "Windowed";
    logsManager.logMessage("Window and renderer validated. Mode: " + mode + ", Size: " + std::to_string(width) + "x" + std::to_string(height));

   
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (logsManager.checkAndLogError(!renderer, "Renderer could not be created! SDL_Error: " + std::string(SDL_GetError()))) {
      return false;
    }
    logsManager.logMessage("Renderer created successfully.");


    initialized = true;
    return true;
  }

  void SDLManager::clear() {
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
