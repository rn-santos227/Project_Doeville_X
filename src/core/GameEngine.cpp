#include "GameEngine.h"

namespace Project::Core {
  GameEngine::GameEngine() : 
    isRunning(false), logsManager(), 
    cursorHandler(logsManager, ResourcesHandler::getResourcePath("resources/system")), keyHandler(logsManager), mouseHandler(logsManager), 
    fontHandler(logsManager), 
    gameStateManager(),
    framesCounter(), 
    screenHandler(gameStateManager, fontHandler, keyHandler, mouseHandler, logsManager, framesCounter),
    scriptingService(gameStateManager, logsManager)
    {}

  GameEngine::~GameEngine() {

  }

  void GameEngine::init() {
    std::string cursorPath = "cursor_default.bmp";

    std::string fontPath = ResourcesHandler::getResourcePath("resources/fonts/system.ttf");
    
    if (logsManager.checkAndLogError(!screenHandler.init(), "Screen Handler initialization failed!")) {
      isRunning = false;
      logsManager.flushLogs();
      return;
    }

    if (logsManager.checkAndLogError(!fontHandler.loadFont("system", fontPath.c_str(), 24), "Failed to load required font 'system'!")) {
      logsManager.flushLogs();
      return;
    }
    
    scriptingService.loadScriptsFromFolder("scripts/");

    keyHandler.setKeyBinding(KeyAction::HELP_TOGGLE, SDL_SCANCODE_F1);
    logsManager.logMessage("Game Engine has been initialized successfully.");
    
    logsManager.flushLogs();
    isRunning = true;
  }

  void GameEngine::run() {
    Uint32 frameStartTime;
    Uint32 previousTime = SDL_GetTicks();
    float deltaTime = 0.0f;

    while (isRunning) {
      frameStartTime = SDL_GetTicks();

      deltaTime = (frameStartTime - previousTime) / 1000.0f;
      previousTime = frameStartTime;

      handleEvents();
      update();
      
      gameStateManager.update(deltaTime);
      framesCounter.update();
      
      render();
      
      Uint32 frameDuration = SDL_GetTicks() - frameStartTime;
      if (frameDuration < 16) {  
        SDL_Delay(16 - frameDuration); 
      }
    }
    clean();
  }

  void GameEngine::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event) != 0) {
      keyHandler.handleInput(event); 

      if (event.type == SDL_QUIT) {
        gameStateManager.cleanup();
        logsManager.logMessage("Quit event received");
        logsManager.flushLogs();
        isRunning = false;
      }
    }
  }

  void GameEngine::update() {
    std::lock_guard<std::mutex> lock(updateMutex);
    screenHandler.update();
    screenHandler.clear();
  }

  void GameEngine::render() {
    screenHandler.render();
  }

  void GameEngine::handleFrameRate() {
    //For Optimization
  }

  void GameEngine::clean() {
    logsManager.logMessage("Cleaning up game engine...");
    
    gameStateManager.cleanup();
    SDL_Quit();
  }
}