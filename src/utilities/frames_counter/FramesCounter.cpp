#include "FramesCounter.h"

FramesCounter::FramesCounter() : frameCount(0), lastTime(SDL_GetTicks()), fps(0) {}

void FramesCounter::update() {
  frameCount++;
  Uint32 currentTime = SDL_GetTicks();

  if (currentTime - lastTime >= 1000) {
    fps = frameCount;
    frameCount = 0;
    lastTime = currentTime;
  }
}

int FramesCounter::getFPS() const {
  return fps;
}