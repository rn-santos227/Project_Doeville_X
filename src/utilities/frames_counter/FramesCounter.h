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
  int frameCount;
  Uint32 lastTime;
  int fps;
};

#endif