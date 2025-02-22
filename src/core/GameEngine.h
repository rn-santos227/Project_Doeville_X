#ifndef GAME_ENGINE_H
#define GAME_ENGINE_H

#include <iostream>
#include <mutex>

#include <SDL.h>

#include "handlers/font/FontHandler.h"
#include "handlers/input/CursorHandler.h"
#include "handlers/input/KeyHandler.h"
#include "handlers/input/MouseHandler.h"
#include "handlers/resources/ResourcesHandler.h"
#include "handlers/screen/ScreenHandler.h"

#include "states/GameStateManager.h"

#include "services/scripting/ScriptingService.h"

#include "utilities/logs_manager/LogsManager.h"
#include "utilities/frames_counter/FramesCounter.h"

using namespace Project::Handlers;
using namespace Project::Services;
using namespace Project::States;
using namespace Project::Utilities;

namespace Project::Core {
  class GameEngine {
  public:
    GameEngine();
    ~GameEngine();

    void init();
    void run();
    void clean();

  private:
    CursorHandler cursorHandler;
    FontHandler fontHandler;
    KeyHandler keyHandler;
    MouseHandler mouseHandler;
    ResourcesHandler resourcesHandler;
    ScreenHandler screenHandler;

    GameStateManager gameStateManager;

    ScriptingService scriptingService;

    LogsManager logsManager;
    FramesCounter framesCounter;

    bool isRunning;

    void handleEvents();
    void update();
    void render();
    void handleFrameRate();

    std::mutex eventMutex;
    std::mutex updateMutex;
  };
}

#endif