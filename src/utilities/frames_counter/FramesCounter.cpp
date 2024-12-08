#include "FramesCounter.h"

FramesCounter::FramesCounter() : lastTime(SDL_GetPerformanceCounter()), frequency(SDL_GetPerformanceFrequency()), frameCount(0), fps(0) {}

void FramesCounter::update() {
  frameCount++;
  Uint64 currentTime = SDL_GetPerformanceCounter();

  if (currentTime - lastTime >= 1000) {
    fps = frameCount;
    frameCount = 0;
    lastTime = currentTime;
  }
}

int FramesCounter::getFPS() const {
  return fps;
}