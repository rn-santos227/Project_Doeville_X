#ifndef SDL_MANAGER_H
#define SDL_MANAGER_H

#include <SDL.h>
#include <string>

namespace Project::Core {
  class SDLManager {
  public:
    SDLManager();
    ~SDLManager();

    bool initialize(const std::string& title, int width, int height, bool fullscreen);
    void clear();
    void present();

    SDL_Renderer* getRenderer() const;
  };
}

#endif