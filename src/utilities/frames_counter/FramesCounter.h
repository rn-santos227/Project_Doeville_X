#ifndef FRAME_COUNTER_H
#define FRAME_COUNTER_H

#include <iostream>

#include <SDL.h>

namespace Project::Utilities {
  class FramesCounter {
  public:
    FramesCounter();
    ~FramesCounter() = default;

    void update();
    
    double getDeltaTime() const;
    int getFPS() const; 

  private:
    Uint64 lastTime;
    Uint64 frequency;

    double deltaTime;

    int frameCount;
    int fps;
  };
}

#endif