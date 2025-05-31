#ifndef SDL_MANAGER_H
#define SDL_MANAGER_H

#include <SDL.h>
#include <string>

#include "utilities/logs_manager/LogsManager.h"
using namespace Project::Utilities;

namespace Project::Core {
  class SDLManager {
  public:
    SDLManager();
    ~SDLManager();

    bool initialize(const std::string& title, int width, int height, bool fullscreen);
    void clear();
    void present();

    SDL_Renderer* getRenderer() const;
    SDL_Window* getWindow() const;

  private:
    LogsManager& logsManager;
    
    SDL_Window* window;
    SDL_Renderer* renderer;

    bool isInitialized;
    void cleanup();
  };
}

#endif