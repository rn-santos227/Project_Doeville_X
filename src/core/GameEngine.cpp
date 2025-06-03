#include "GameEngine.h"

namespace Project::Core {
  GameEngine::GameEngine() :
  isRunning(false), framesCounter(), logsManager(), configReader(logsManager), sdlManager(logsManager),
  componentsFactory(std::make_unique<ComponentsFactory>(configReader, logsManager)),
  gameStateManager(std::make_unique<GameStateManager>(5, logsManager)),
  cursorHandler(std::make_unique<CursorHandler>(logsManager)), 
  fontHandler(std::make_unique<FontHandler>(logsManager)), 
  keyHandler(std::make_unique<KeyHandler>(logsManager)),
  mouseHandler(std::make_unique<MouseHandler>(logsManager)),
  resourcesHandler(std::make_unique<ResourcesHandler>(logsManager))
  {}
  
  GameEngine::~GameEngine() {
    if (isRunning) {
      clean();
    }
  }

  void GameEngine::init() {
    std::string title = configReader.getValue("Window", "title", "Project Doeville X");
    int screenWidth = configReader.getIntValue("Window", "width", 800);
    int screenHeight = configReader.getIntValue("Window", "height", 600);
    bool isFullscreen = configReader.getBoolValue("Window", "fullscreen", false);

    if (!sdlManager.initialize(title, screenWidth, screenHeight, isFullscreen)) {
      logsManager.logError("Failed to initialize SDLManager.");
      return;
    }

    SDL_ShowCursor(SDL_DISABLE);

    if (logsManager.checkAndLogError(!configReader.loadConfig("config.ini"), "Failed to load config.ini")) {
      logsManager.flushLogs();
      return;
    }

    std::string fontPath = resourcesHandler->getResourcePath("resources/fonts/system.ttf");
    
    if (logsManager.checkAndLogError(!screenHandler->init(), "Screen Handler initialization failed!")) {
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
    
    componentsFactory->setRenderer(screenHandler->getRenderer()); 
    keyHandler->setKeyBinding(KeyAction::HELP_TOGGLE, SDL_SCANCODE_F1);
    
    logsManager.logMessage("Game Engine has been initialized successfully.");
    logsManager.flushLogs();
    isRunning = true;
  }

  void GameEngine::run() {
    Uint32 frameStartTime;
    Uint32 previousTime = SDL_GetTicks();
    double deltaTime = 0.0f;

    while (isRunning) {
      frameStartTime = SDL_GetTicks();

      deltaTime = (frameStartTime - previousTime) / 1000.0f;
      previousTime = frameStartTime;

      handleEvents();
      update(deltaTime);
      
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

  void GameEngine::update(float deltaTime) {
    std::lock_guard<std::mutex> lock(updateMutex);
    gameStateManager->update(deltaTime);
    sdlManager.clear();
  }

  void GameEngine::render() {
    screenHandler->render();
  }

  void GameEngine::handleFrameRate() {
    //For Optimization
  }

  void GameEngine::clean() {
    logsManager.logMessage("Cleaning up game engine...");

    gameStateManager->cleanup();
    cursorHandler->cleanup();
    fontHandler->cleanup();
    
    IMG_Quit();
    isRunning = false;

    logsManager.logMessage("Game engine cleanup complete.");
    logsManager.flushLogs();
  }
}