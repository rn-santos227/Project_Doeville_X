#include "FramesCounter.h"

namespace Project::Utilities {
  FramesCounter::FramesCounter() : lastTime(SDL_GetPerformanceCounter()), frequency(SDL_GetPerformanceFrequency()), deltaTime(0.0), frameCount(0), fps(0) {}

  void FramesCounter::update() {
    Uint64 currentTime = SDL_GetPerformanceCounter();
    deltaTime = (currentTime - lastTime) / static_cast<double>(frequency);
    frameCount++;

    
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

  double FramesCounter::getDeltaTime() const {
    return deltaTime;
  }
}
