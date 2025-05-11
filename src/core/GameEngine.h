#ifndef GAME_ENGINE_H
#define GAME_ENGINE_H

#include <iostream>
#include <mutex>

#include <SDL.h>

#include "factories/component/ComponentsFactory.h"

#include "handlers/config/ConfigHandler.h"
#include "handlers/font/FontHandler.h"
#include "handlers/input/CursorHandler.h"
#include "handlers/input/KeyHandler.h"
#include "handlers/input/MouseHandler.h"
#include "handlers/resources/ResourcesHandler.h"
#include "handlers/screen/ScreenHandler.h"

#include "states/GameStateManager.h"

#include "utilities/config_reader/ConfigReader.h"
#include "utilities/logs_manager/LogsManager.h"
#include "utilities/frames_counter/FramesCounter.h"

using namespace Project::Factories;
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
    void handleEvents();
    void update(float deltaTime);
    void render();
    void handleFrameRate();
    
    bool isRunning;
    LogsManager logsManager;
    FramesCounter framesCounter;
    ConfigReader configReader;

    std::unique_ptr<ComponentsFactory> componentFactory;
    std::unique_ptr<GameStateManager> gameStateManager;

    std::unique_ptr<ConfigHandler> configHandler;
    std::unique_ptr<CursorHandler> cursorHandler;
    std::unique_ptr<FontHandler> fontHandler;
    std::unique_ptr<KeyHandler> keyHandler;
    std::unique_ptr<MouseHandler> mouseHandler;
    std::unique_ptr<ResourcesHandler> resourcesHandler;
    std::unique_ptr<ScreenHandler> screenHandler;

    std::mutex updateMutex;
  };
}

#endif