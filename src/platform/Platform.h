#ifndef PLATFORM_H
#define PLATFORM_H

#include <memory>
#include <string>

struct SDL_Renderer;
struct SDL_Window;

#include "interfaces/cleanup_interface/Cleanable.h"

namespace Project::Platform {
  class RendererAPI;
  class Platform: public Project::Interfaces::Cleanable {
  public:
    virtual ~Platform() = default;  
    virtual SDL_Renderer* getRenderer() const = 0;
    virtual SDL_Window* getWindow() const = 0;

    virtual bool init(const std::string& title, int width, int height, bool fullscreen, bool vsync, bool useOpenGL) = 0;
    virtual void present() = 0;
    virtual void cleanup() = 0;
    virtual void clear() = 0;

    virtual void requestExit() = 0;
    virtual bool isExitRequested() const = 0;
    virtual void clearExitRequest() = 0;

    virtual void setRendererAPI(std::unique_ptr<RendererAPI> api) = 0;
  };
}

#endif
