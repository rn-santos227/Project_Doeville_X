#include "GameEngine.h"

namespace Project::Core {
  GameEngine::GameEngine() :
  isRunning(false), logsManager(), framesCounter(), 
  cursorHandler(std::make_unique<CursorHandler>(logsManager)), 
  fontHandler(std::make_unique<FontHandler>(logsManager)), 
  keyHandler(std::make_unique<KeyHandler>(logsManager)),
  mouseHandler(std::make_unique<MouseHandler>(logsManager)),
  resourcesHandler(std::make_unique<ResourcesHandler>()),
  gameStateManager(5, logsManager),
  screenHandler(gameStateManager, *cursorHandler, *fontHandler, *keyHandler, *mouseHandler, *resourcesHandler, logsManager, framesCounter),
  scriptingService(gameStateManager, logsManager) {}
  
  GameEngine::~GameEngine() {}

  void GameEngine::init() {
    SDL_ShowCursor(SDL_DISABLE);
    std::string fontPath = ResourcesHandler::getResourcePath("resources/fonts/system.ttf");
    
    if (logsManager.checkAndLogError(!screenHandler.init(), "Screen Handler initialization failed!")) {
      isRunning = false;
      logsManager.flushLogs();
      return;
    }

    if (logsManager.checkAndLogError(!fontHandler->loadFont("system", fontPath.c_str(), 24), "Failed to load required font 'system'!")) {
      logsManager.flushLogs();
      return;
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
      logsManager.logError("Failed to initialize SDL_image for PNG: " + std::string(IMG_GetError()));
      return;
    }
    
    scriptingService.loadScriptsFromFolder("scripts/");

    keyHandler->setKeyBinding(KeyAction::HELP_TOGGLE, SDL_SCANCODE_F1);
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
  }

  void GameEngine::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event) != 0) {
      keyHandler->handleInput(event); 

      if (event.type == SDL_QUIT) {
        logsManager.logMessage("Quit event received");
        clean();
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
    cursorHandler->cleanup();
    fontHandler->cleanup();
    
    IMG_Quit();
    SDL_Quit();
    isRunning = false;

    logsManager.logMessage("Game engine cleanup complete.");
    logsManager.flushLogs();
  }
}