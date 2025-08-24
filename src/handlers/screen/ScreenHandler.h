#ifndef SCREEN_HANDLER_H
#define SCREEN_HANDLER_H

#include "DebugDisplay.h"

#include <iostream>
#include <memory>
#include <mutex>

#include "factories/component/ComponentsFactory.h"
#include "handlers/camera/CameraHandler.h"
#include "handlers/font/FontHandler.h"
#include "handlers/input/CursorHandler.h"
#include "handlers/input/KeyHandler.h"
#include "handlers/input/MouseHandler.h"
#include "handlers/resources/ResourcesHandler.h"
#include "libraries/constants/Constants.h"
#include "platform/Platform.h"
#include "services/scripting/ScriptingService.h"
#include "services/styling/StyleService.h"
#include "states/GameStateManager.h"
#include "utilities/config_reader/ConfigReader.h"
#include "utilities/frames_counter/FramesCounter.h"
#include "utilities/logs_manager/LogsManager.h"

namespace Project::Handlers {
  class ScreenHandler {
  public:
    ScreenHandler(Project::Utilities::LogsManager& logsManager, 
      Project::Utilities::FramesCounter& frameCounter,
      Project::Utilities::ConfigReader& configReader,
      Project::Platform::Platform& platform,
      Project::Factories::ComponentsFactory& componentsFactory, 
      Project::States::GameStateManager& gameStateManager, 
      Project::Handlers::CursorHandler& cursorHandler, 
      Project::Handlers::FontHandler& fontHandler, 
      Project::Handlers::KeyHandler& keyHandler, 
      Project::Handlers::MouseHandler& mouseHandler, 
      Project::Handlers::ResourcesHandler& resourcesHandler);
    ~ScreenHandler();

    bool init();
    void render();
    void handleEvents();
    
    bool isRunning() const;

    CameraHandler* getCameraHandler() const;
    SDL_Renderer* getRenderer() const;
    SDL_Window* getWindow() const;

  private:
    Project::Utilities::LogsManager& logsManager;
    Project::Utilities::FramesCounter& framesCounter;
    Project::Utilities::ConfigReader& configReader;
    Project::Platform::Platform& platform;

    Project::Factories::ComponentsFactory& componentsFactory;
    Project::States::GameStateManager& gameStateManager;

    Project::Handlers::CursorHandler& cursorHandler;
    Project::Handlers::FontHandler& fontHandler;
    Project::Handlers::KeyHandler& keyHandler;
    Project::Handlers::MouseHandler& mouseHandler;
    Project::Handlers::ResourcesHandler& resourcesHandler;

    std::unique_ptr<Project::Services::ScriptingService> scriptingService;
    std::unique_ptr<Project::Services::StyleService> styleService;

    std::mutex renderMutex;
    std::mutex eventMutex;

    Project::Handlers::CameraHandler cameraHandler;
    Project::Handlers::DebugDisplay debugDisplay;

    bool running;
    int cursorWidth;
    int cursorHeight;
  };
}

#endif
