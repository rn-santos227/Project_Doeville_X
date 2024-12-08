#ifndef SCREENHANDLER_H
#define SCREENHANDLER_H

#include <iostream>

#include <SDL2/SDL.h>

#include "utilities/frames_counter/FramesCounter.h"

class ScreenHandler {
public:
  ScreenHandler();
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

  FramesCounter framesCounter;
  bool debugMode;
  bool running;
};

#endif