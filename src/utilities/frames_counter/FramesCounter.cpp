#include "FramesCounter.h"

namespace Project::Utilities {
  FramesCounter::FramesCounter() : lastTime(SDL_GetPerformanceCounter()), frequency(SDL_GetPerformanceFrequency()), frameCount(0), fps(0) {}

  void FramesCounter::update() {
    frameCount++;
    Uint64 currentTime = SDL_GetPerformanceCounter();
    
    double elapsedTime = (currentTime - lastTime) / (double)frequency * 1000;

    if (elapsedTime >= 1000.0) {
      fps = frameCount;
      frameCount = 0;
      lastTime = currentTime;
    }
  }

  int FramesCounter::getFPS() const {
    return fps;
  }
}
