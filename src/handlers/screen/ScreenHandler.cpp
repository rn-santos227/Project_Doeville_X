#include "ScreenHandler.h"

#include <iostream>
#include <SDL2/SDL_ttf.h>

ScreenHandler::ScreenHandler(FontHandler& fontHandler, InputHandler& inputHandler, FramesCounter& framesCounter)
    : window(nullptr), renderer(nullptr), running(false), fontHandler(fontHandler), inputHandler(inputHandler), framesCounter(framesCounter) {}

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
      renderFPS();
    }
  } else {
    std::cerr << "Renderer is null!" << std::endl;
  }
}

void ScreenHandler::update() {
  SDL_RenderPresent(renderer); 
}

void ScreenHandler::handleEvents() {
  SDL_Event e;
  while (SDL_PollEvent(&e) != 0) {
    if (e.type == SDL_QUIT) {
      running = false;
    }
    inputHandler.handleInput(e);
  }
}

bool ScreenHandler::isRunning() const {
  return running;
}

void ScreenHandler::renderFPS() {
  int fps = framesCounter.getFPS();
  std::string fpsText = "FPS: " + std::to_string(fps);
  
  SDL_Color color = {144, 238, 144, 255};
  SDL_Texture* fpsTexture = fontHandler.renderText(renderer, fpsText, "doeville", color);
  
  if (fpsTexture) {
    int textWidth, textHeight;
    SDL_QueryTexture(fpsTexture, nullptr, nullptr, &textWidth, &textHeight);

    SDL_Rect destRect = {800 - textWidth - 10, 10, textWidth, textHeight};
    SDL_RenderCopy(renderer, fpsTexture, nullptr, &destRect);
    SDL_DestroyTexture(fpsTexture);
  } else {
    std::cerr << "Failed to render FPS text!" << std::endl;
  }
}