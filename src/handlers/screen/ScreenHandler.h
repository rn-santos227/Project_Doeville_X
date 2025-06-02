#ifndef SCREEN_HANDLER_H
#define SCREEN_HANDLER_H

#include <iostream>
#include <memory>
#include <mutex>

#include <SDL.h>

#include "core/SDLManager.h"

#include "factories/component/ComponentsFactory.h"

#include "handlers/font/FontHandler.h"
#include "handlers/input/CursorHandler.h"
#include "handlers/input/KeyHandler.h"
#include "handlers/input/MouseHandler.h"
#include "handlers/resources/ResourcesHandler.h"

#include "services/scripting/ScriptingService.h"

#include "states/GameStateManager.h"

#include "utilities/config_reader/ConfigReader.h"
#include "utilities/frames_counter/FramesCounter.h"
#include "utilities/logs_manager/LogsManager.h"

using namespace Project::Core;
using namespace Project::Factories;
using namespace Project::Services;
using namespace Project::States;
using namespace Project::Utilities;

namespace Project::Handlers {
  class ScreenHandler {
  public:
    ScreenHandler(LogsManager& logsManager, FramesCounter& frameCounter, ConfigReader& configReader, ComponentsFactory& componentsFactory, GameStateManager& gameStateManager, CursorHandler& cursorHandler, FontHandler& fontHandler, KeyHandler& keyHandler, MouseHandler& mouseHandler, ResourcesHandler& resourcesHandler);
    ~ScreenHandler();

    bool init();
    void clear();
    void render();
    void handleEvents();
    
    bool isRunning() const;

    SDL_Renderer* getRenderer() const;

  private:
    SDL_Window* window;
    SDL_Renderer* renderer;

    LogsManager& logsManager;
    FramesCounter& framesCounter;
    ConfigReader& configReader;
    SDLManager& sdlManager;
    
    ComponentsFactory& componentsFactory;
    GameStateManager& gameStateManager;

    CursorHandler& cursorHandler;
    FontHandler& fontHandler;
    KeyHandler& keyHandler;
    MouseHandler& mouseHandler;
    ResourcesHandler& resourcesHandler;
    
    std::unique_ptr<ScriptingService> scriptingService;

    std::mutex renderMutex;
    std::mutex eventMutex;
    
    bool running;
    int cursorWidth;
    int cursorHeight;
    
    void renderFPS();
    void renderMousePosition();
  };
}

#endif