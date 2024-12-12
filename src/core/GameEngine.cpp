#include "GameEngine.h"

GameEngine::GameEngine() : 
  window(nullptr), renderer(nullptr), isRunning(false), logsManager(), 
  keyHandler(logsManager), mouseHandler(logsManager), 
  fontHandler(logsManager), framesCounter(), 
  screenHandler(fontHandler, keyHandler, mouseHandler, logsManager, framesCounter) {}

GameEngine::~GameEngine() {}

void GameEngine::init() {
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

  keyHandler.setKeyBinding(KeyAction::HELP_TOGGLE, SDL_SCANCODE_F1);
  logsManager.logMessage("Game Engine has been initialized successfully.");
  
  logsManager.flushLogs();
  isRunning = true;
}

void GameEngine::run() {
  Uint32 frameStartTime;

  while (isRunning) {
    frameStartTime = SDL_GetTicks();

    handleEvents();
    update();

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
      logsManager.logMessage("Quit event received");
      logsManager.flushLogs();
      isRunning = false;
    }
  }
}

void GameEngine::update() {
  screenHandler.clear();
}

void GameEngine::render() {
  screenHandler.render();
  screenHandler.update(); 
}

void GameEngine::handleFrameRate() {
  //For Optimization
}

void GameEngine::clean() {
  SDL_Quit();
}