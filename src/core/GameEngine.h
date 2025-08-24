#ifndef GAME_ENGINE_H
#define GAME_ENGINE_H

#include <iostream>
#include <utility>
#include <vector>
#include <memory>

#include "factories/component/ComponentsFactory.h"
#include "handlers/font/FontHandler.h"
#include "handlers/input/CursorHandler.h"
#include "handlers/input/KeyHandler.h"
#include "handlers/input/MouseHandler.h"
#include "handlers/resources/ResourcesHandler.h"
#include "handlers/screen/ScreenHandler.h"
#include "interfaces/cleanup_interface/Cleanable.h"
#include "platform/Platform.h"
#include "platform/sdl/SDLPlatform.h"
#include "services/cache/SceneCacheService.h"
#include "states/GameStateManager.h"
#include "utilities/config_reader/ConfigReader.h"
#include "utilities/logs_manager/LogsManager.h"
#include "utilities/frames_counter/FramesCounter.h"

namespace Project::Core {
  class GameEngine {
  public:
    GameEngine();
    ~GameEngine();

    void init();
    void run();
    void clean();

  private:
    Project::Utilities::LogsManager logsManager;
    Project::Utilities::FramesCounter framesCounter;
    Project::Utilities::ConfigReader configReader;
    
    std::unique_ptr<Project::Platform::Platform> platform;
    std::unique_ptr<Project::Handlers::ResourcesHandler> resourcesHandler;
    std::unique_ptr<Project::Factories::ComponentsFactory> componentsFactory;
    std::unique_ptr<Project::Services::SceneCacheService> sceneCache;
    std::unique_ptr<Project::States::GameStateManager> gameStateManager;

    std::unique_ptr<Project::Handlers::CursorHandler> cursorHandler;
    std::unique_ptr<Project::Handlers::FontHandler> fontHandler;
    std::unique_ptr<Project::Handlers::KeyHandler> keyHandler;
    std::unique_ptr<Project::Handlers::MouseHandler> mouseHandler;
    std::unique_ptr<Project::Handlers::ScreenHandler> screenHandler;

    std::vector<std::pair<Project::Interfaces::Cleanable*, std::string>> cleanupHandlers;

    double entityLoadFactor;
    double frameTimeAvg;
    double maxFPS;

    bool isRunning;

    void handleEvents();
    void update(float deltaTime);
    void render();
    void handleFrameRate(Uint64 frameStartTime);
  };
}

#endif
