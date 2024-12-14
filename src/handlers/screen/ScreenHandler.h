#ifndef SCREEN_HANDLER_H
#define SCREEN_HANDLER_H

#include <iostream>

#include <SDL.h>

#include "handlers/font/FontHandler.h"
#include "handlers/input/KeyHandler.h"
#include "handlers/input/MouseHandler.h"

#include "states/GameStateManager.h"

#include "utilities/frames_counter/FramesCounter.h"
#include "utilities/logs_manager/LogsManager.h"

using namespace Project::Utilities;

namespace Project::Handlers {
  class ScreenHandler {
  public:
    ScreenHandler(GameStateManager& gameStateManager, FontHandler& fontHandler, KeyHandler& keyHandler, MouseHandler& mouseHandler, LogsManager& logsManager, FramesCounter& frameCounter);
    ~ScreenHandler();

    bool init();
    void clear();
    void render();
    void update();
    void handleEvents();

    bool isRunning() const;

  private:
    SDL_Window* window;
    SDL_Renderer* renderer;

    FontHandler& fontHandler;
    KeyHandler& keyHandler;
    MouseHandler& mouseHandler;

    GameStateManager& gameStateManager;
    
    LogsManager& logsManager;
    FramesCounter& framesCounter;
    
    bool running;
    void renderFPS();
    void renderMousePosition();
  };
}

#endif