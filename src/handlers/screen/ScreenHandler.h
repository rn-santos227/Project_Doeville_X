#ifndef SCREENHANDLER_H
#define SCREENHANDLER_H

#include <iostream>

#include <SDL2/SDL.h>

#include "handlers/font/FontHandler.h"
#include "handlers/input/InputHandler.h"

#include "utilities/frames_counter/FramesCounter.h"

class ScreenHandler {
public:
  ScreenHandler(FontHandler& fontHandler, InputHandler& inputHandler);
  ~ScreenHandler();

  bool init();
  void clear();
  void render();
  void update();
  void handleEvents();

  bool isRunning() const;

private:
  SDL_Window* window;
  SDL_Renderer* renderer;

  FontHandler& fontHandler;
  InputHandler& inputHandler;

  FramesCounter framesCounter;
  
  bool debugMode;
  bool running;

  void renderFPS();
};

#endif