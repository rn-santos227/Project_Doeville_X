#ifndef GAMEENGINE_H
#define GAMEENGINE_H

#include <iostream>

#include <SDL2/SDL.h>

#include "handlers/input/InputHandler.h"
#include "handlers/font/FontHandler.h"
#include "handlers/resources/ResourcesHandler.h"
#include "handlers/screen/ScreenHandler.h"

#include "utilities/frames_counter/FramesCounter.h"

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
  FontHandler fontHandler;
  ResourcesHandler resourcesHandler;
  ScreenHandler screenHandler;

  FramesCounter framesCounter; 

  void handleEvents();
  void update();
  void render();

  void handleFrameRate();
};

#endif