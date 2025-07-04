#ifndef DEBUG_DISPLAY_H
#define DEBUG_DISPLAY_H

#include <SDL.h>
#include <string>

#include "core/SDLManager.h"
#include "handlers/font/FontHandler.h"
#include "handlers/input/MouseHandler.h"
#include "libraries/constants/Constants.h"
#include "utilities/config_reader/ConfigReader.h"
#include "utilities/frames_counter/FramesCounter.h"
#include "utilities/logs_manager/LogsManager.h"

namespace Constants = Project::Libraries::Constants;

namespace Project::Handlers {
  class DebugDisplay {
  public:
    DebugDisplay(Project::Utilities::LogsManager& logsManager,
      Project::Utilities::FramesCounter& framesCounter,
      Project::Utilities::ConfigReader& configReader,
      Project::Core::SDLManager& sdlManager,
      Project::Handlers::FontHandler& fontHandler,
      Project::Handlers::MouseHandler& mouseHandler);
    ~DebugDisplay() = default;

    void render();

  private:
    Project::Utilities::LogsManager& logsManager;
    Project::Utilities::FramesCounter& framesCounter;
    Project::Utilities::ConfigReader& configReader;
    Project::Core::SDLManager& sdlManager;
    Project::Handlers::FontHandler& fontHandler;
    Project::Handlers::MouseHandler& mouseHandler;

    SDL_Color debugTextColor = Constants::DEFAULT_DEBUG_TEXT_COLOR;
    SDL_Color axisXColor = Constants::DEFAULT_X_AXIS_COLOR;
    SDL_Color axisYColor = Constants::DEFAULT_Y_AXIS_COLOR;

    int fpsTextHeight = 0;

    void renderFPS();
    void renderBenchmark();
    void renderMousePosition();
    void renderAxes();    
  };
}
#endif
