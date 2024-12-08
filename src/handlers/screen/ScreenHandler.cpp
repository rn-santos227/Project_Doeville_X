#include <iostream>
#include <SDL2/SDL_ttf.h>

#include "ScreenHandler.h"

ScreenHandler::ScreenHandler(FontHandler& fontHandler, InputHandler& inputHandler)
    : window(nullptr), renderer(nullptr), running(false), fontHandler(fontHandler), inputHandler(inputHandler) {}

ScreenHandler::~ScreenHandler() {
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
}

bool ScreenHandler::init() {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
    return false;
  }
  std::cout << "SDL Initialized successfully!" << std::endl;

  window = SDL_CreateWindow("Project Doeville X", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, SDL_WINDOW_SHOWN);
  if (!window) {
    std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
    return false;
  }
  std::cout << "Window created successfully!" << std::endl;

  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if (!renderer) {
    std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
    return false;
  }
  std::cout << "Renderer created successfully!" << std::endl;

  running = true;
  return true;
}

void ScreenHandler::clear() {
  if (renderer) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
  } else {
    std::cerr << "Renderer is null!" << std::endl;
  }
}

void ScreenHandler::render() {
  if (renderer) {
    if (inputHandler.isGameDebugMode()) {
      
    }
  } else {
    std::cerr << "Renderer is null!" << std::endl;
  }
}

void ScreenHandler::update() {
  framesCounter.update();
  SDL_RenderPresent(renderer); 
}

void ScreenHandler::handleEvents() {
  SDL_Event e;
  while (SDL_PollEvent(&e) != 0) {
    if (e.type == SDL_QUIT) {
      running = false;
    }
  }
}

bool ScreenHandler::isRunning() const {
  return running;
}

void ScreenHandler::renderFPS() {

}