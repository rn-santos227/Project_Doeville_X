#include "GameEngine.h"

GameEngine::GameEngine() : window(nullptr), renderer(nullptr), isRunning(false), inputHandler(), fontHandler(), framesCounter(), screenHandler(fontHandler, inputHandler, framesCounter) {}

GameEngine::~GameEngine() {}

void GameEngine::init() {
  std::string fontPath = ResourcesHandler::getResourcePath("resources/fonts/doeville.ttf");
  
  if (!screenHandler.init()) {
    std::cerr << "ScreenHandler initialization failed!" << std::endl;
    isRunning = false;
    return;
  }

  if (!fontHandler.loadFont("doeville", fontPath.c_str(), 24)) {
    std::cerr << "Failed to load required font 'doeville'!" << std::endl;
    return;
  }

  inputHandler.setKeyBinding(InputAction::HELP_TOGGLE, SDL_SCANCODE_F1);
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
    inputHandler.handleInput(event); 

    if (event.type == SDL_QUIT) {
      std::cout << "Quit event received" << std::endl;
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