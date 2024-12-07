#ifndef GAMEENGINE_H
#define GAMEENGINE_H

#include <iostream>
#include <chrono>

class GameEngine {
public:
  GameEngine();
  ~GameEngine();

  void init();
  void run();
  void update();
  void render();

};

#endif