#ifndef SDL_PLATFORM_H
#define SDL_PLATFORM_H

#include <SDL.h>
#include <string>
#include <memory>

#include "platform/Platform.h"
#include "platform/renderer/RendererAPI.h"
#include "utilities/logs_manager/LogsManager.h"

namespace Project::Platform {
  class SDLPlatform : public Platform {
  public:
    explicit SDLPlatform(Project::Utilities::LogsManager& logsManager);
    ~SDLPlatform();

    SDL_Renderer* getRenderer() const override;
    SDL_Window* getWindow() const override;

    bool init(const std::string& title, int width, int height, bool fullscreen, bool vsync, bool useOpenGL) override;
    void present() override;    
    void cleanup() override;
    void clear() override;

    void requestExit() override;
    bool isExitRequested() const override;
    void clearExitRequest() override;

    void setRendererAPI(std::unique_ptr<RendererAPI> api);

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
