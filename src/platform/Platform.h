#ifndef PLATFORM_H
#define PLATFORM_H

#include <string>

struct SDL_Renderer;
struct SDL_Window;

namespace Project::Platform {
  class Platform {
  public:
    virtual ~Platform() = default;  
    virtual bool init(const std::string& title, int width, int height, bool fullscreen, bool vsync, bool useOpenGL) = 0;

    virtual void clear() = 0;
    virtual void present() = 0;

    virtual SDL_Renderer* getRenderer() const = 0;
    virtual SDL_Window* getWindow() const = 0;
  };
}

#endif
