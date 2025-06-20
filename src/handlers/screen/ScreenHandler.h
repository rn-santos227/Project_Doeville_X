#ifndef SCREEN_HANDLER_H
#define SCREEN_HANDLER_H

#include <iostream>
#include <memory>
#include <mutex>

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
    ScreenHandler(LogsManager& logsManager, FramesCounter& frameCounter, ConfigReader& configReader, SDLManager& sdlManager, ComponentsFactory& componentsFactory, GameStateManager& gameStateManager, CursorHandler& cursorHandler, FontHandler& fontHandler, KeyHandler& keyHandler, MouseHandler& mouseHandler, ResourcesHandler& resourcesHandler);
    ~ScreenHandler();

    bool init();
    void render();
    void handleEvents();
    
    bool isRunning() const;

    SDL_Renderer* getRenderer() const;
    SDL_Window* getWindow() const;

  private:
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

    SDL_Color debugTextColor{144, 238, 144, 255};
    
    bool running;
    int cursorWidth;
    int cursorHeight;
    
    void renderFPS();
    void renderMousePosition();
  };
}
#endif
