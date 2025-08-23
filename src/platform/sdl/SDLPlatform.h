#ifndef SDL_PLATFORM_H
#define SDL_PLATFORM_H

#include <SDL.h>
#include <string>
#include <memory>

#include "platform/Platform.h"
#include "platform/renderer/RendererAPI.h"
#include "interfaces/cleanup_interface/Cleanable.h"
#include "utilities/logs_manager/LogsManager.h"

namespace Project::Platform {
  class SDLPlatform : public Platform, public Project::Interfaces::Cleanable {
  public:
    explicit SDLPlatform(Project::Utilities::LogsManager& logsManager);
    ~SDLPlatform();

  private:
    Project::Utilities::LogsManager& logsManager;
    std::unique_ptr<RendererAPI> rendererAPI;

    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_GLContext glContext;

    bool exitRequested;
    bool initialized;

    bool openGLMode;
    bool vsyncEnabled;
  };
}

#endif
