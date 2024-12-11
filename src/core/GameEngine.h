#ifndef GAME_ENGINE_H
#define GAME_ENGINE_H

#include <iostream>

#include <SDL.h>

#include "handlers/font/FontHandler.h"
#include "handlers/input/KeyHandler.h"
#include "handlers/input/MouseHandler.h"
#include "handlers/resources/ResourcesHandler.h"
#include "handlers/screen/ScreenHandler.h"

#include "utilities/logs_manager/LogsManager.h"
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

  FontHandler fontHandler;
  KeyHandler keyHandler;
  MouseHandler mouseHandler;
  ResourcesHandler resourcesHandler;
  ScreenHandler screenHandler;

  LogsManager logsManager;
  FramesCounter framesCounter; 

  bool isRunning;

  void handleEvents();
  void update();
  void render();

  void handleFrameRate();
};

#endif