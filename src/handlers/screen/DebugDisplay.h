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

    void init();
    void render();

  private:
    Project::Utilities::LogsManager& logsManager;
    Project::Utilities::FramesCounter& framesCounter;
    Project::Utilities::ConfigReader& configReader;
    Project::Core::SDLManager& sdlManager;
    Project::Handlers::FontHandler& fontHandler;
    Project::Handlers::MouseHandler& mouseHandler;

    SDL_Color debugTextColor = Project::Libraries::Constants::DEFAULT_DEBUG_TEXT_COLOR;
    SDL_Color axisXColor = Project::Libraries::Constants::DEFAULT_X_AXIS_COLOR;
    SDL_Color axisYColor = Project::Libraries::Constants::DEFAULT_Y_AXIS_COLOR;
    SDL_Color gridColor = Project::Libraries::Constants::DEFAULT_GRID_COLOR;

    int gridSpacing = Project::Libraries::Constants::DEFAULT_GRID_SPACING;

    void renderMousePosition();
    void renderAxes();
    void renderGrid();

    size_t getProcessMemoryUsageMB();
    size_t getProcessCount();
  };
}

#endif
