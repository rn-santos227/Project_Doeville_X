#ifndef GAMEENGINE_H
#define GAMEENGINE_H

#include <iostream>

#include <SDL2/SDL.h>
#include "handlers/input/InputHandler.h"
#include "handlers/screen/ScreenHandler.h"

class GameEngine {
public:
  GameEngine();
  ~GameEngine();

  void init();
  void run();
  void clean();

private:
  SDL_Window* window;
  SDL_Renderer* renderer;
  bool isRunning;

  InputHandler inputHandler;
  ScreenHandler screenHandler;

  void handleEvents();
  void update();
  void render();
};

#endif