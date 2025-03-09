#ifndef SCREEN_HANDLER_H
#define SCREEN_HANDLER_H

#include <iostream>
#include <mutex>

#include <SDL.h>

#include "handlers/config/ConfigHandler.h"
#include "handlers/font/FontHandler.h"
#include "handlers/input/CursorHandler.h"
#include "handlers/input/KeyHandler.h"
#include "handlers/input/MouseHandler.h"
#include "handlers/resources/ResourcesHandler.h"

#include "states/GameStateManager.h"

#include "utilities/frames_counter/FramesCounter.h"
#include "utilities/logs_manager/LogsManager.h"

using namespace Project::States;
using namespace Project::Utilities;

namespace Project::Handlers {
  class ScreenHandler {
  public:
    ScreenHandler(GameStateManager& gameStateManager, CursorHandler& cursorHandler, FontHandler& fontHandler, KeyHandler& keyHandler, MouseHandler& mouseHandler, ResourcesHandler& resourcesHandler, LogsManager& logsManager, FramesCounter& frameCounter);
    ~ScreenHandler();

    bool init();
    void clear();
    void render();
    void update();
    void handleEvents();
    
    bool isRunning() const;

    SDL_Renderer* getRenderer() const;

  private:
    SDL_Window* window;
    SDL_Renderer* renderer;


    CursorHandler& cursorHandler;
    FontHandler& fontHandler;
    KeyHandler& keyHandler;
    MouseHandler& mouseHandler;
    ResourcesHandler& resourcesHandler;

    GameStateManager& gameStateManager;
    
    LogsManager& logsManager;
    FramesCounter& framesCounter;

    std::mutex renderMutex;
    std::mutex eventMutex;
    
    bool running;
    void renderFPS();
    void renderMousePosition();
  };
}

#endif