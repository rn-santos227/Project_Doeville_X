#ifndef FRAME_COUNTER_H
#define FRAME_COUNTER_H

#include <SDL2/SDL.h>
#include <iostream>

class FramesCounter {
public:
  FramesCounter();
  ~FramesCounter() = default;

  void update();
  int getFPS() const; 

private:
  Uint64 lastTime;
  Uint64 frequency;
  int frameCount;
  int fps;
};

#endif