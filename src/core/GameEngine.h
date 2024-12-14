#ifndef GAME_ENGINE_H
#define GAME_ENGINE_H

#include <iostream>

#include <SDL.h>

#include "handlers/font/FontHandler.h"
#include "handlers/input/KeyHandler.h"
#include "handlers/input/MouseHandler.h"
#include "handlers/resources/ResourcesHandler.h"
#include "handlers/screen/ScreenHandler.h"

#include "states/GameStateManager.h"

#include "utilities/logs_manager/LogsManager.h"
#include "utilities/frames_counter/FramesCounter.h"

using namespace Project::Handlers;
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
    SDL_Window* window;
    SDL_Renderer* renderer;

    FontHandler fontHandler;
    KeyHandler keyHandler;
    MouseHandler mouseHandler;
    ResourcesHandler resourcesHandler;
    ScreenHandler screenHandler;

    GameStateManager gameStateManager;

    LogsManager logsManager;
    FramesCounter framesCounter; 

    bool isRunning;

    void handleEvents();
    void update();
    void render();

    void handleFrameRate();
  };
}

#endif