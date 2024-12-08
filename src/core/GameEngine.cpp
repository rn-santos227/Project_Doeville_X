#include "GameEngine.h"

GameEngine::GameEngine() : window(nullptr), renderer(nullptr), isRunning(false) {}
GameEngine::~GameEngine() {}

void GameEngine::init() {
  if (!screenHandler.init()) {
    std::cerr << "ScreenHandler initialization failed!" << std::endl;
    isRunning = false;
    return;
  }

  if (!fontHandler.loadFont("doeville", "/resources/font/doeville.ttf", 24)) {
    std::cerr << "Failed to load required font 'doeville'!" << std::endl;
    return;
  }

  inputHandler.setKeyBinding(InputAction::HELP_TOGGLE, SDL_SCANCODE_F1);
  isRunning = true;
}

void GameEngine::run() {
  while (isRunning) {
    handleEvents();
    update();
    render();
    SDL_Delay(16);
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

void GameEngine::clean() {
  SDL_Quit();
}