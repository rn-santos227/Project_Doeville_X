#include "FramesCounter.h"
#include "libraries/constants/Constants.h"

namespace Project::Utilities {
  namespace Constants = Project::Libraries::Constants;

  FramesCounter::FramesCounter()
    : lastTime(SDL_GetPerformanceCounter()),
      frequency(SDL_GetPerformanceFrequency()),
      startFpsTime(lastTime),
      deltaTime(0.0),
      frameCount(0),
      fps(0),
      startTime(lastTime) {}

  void FramesCounter::update() {
    Uint64 currentTime = SDL_GetPerformanceCounter();
    deltaTime = (currentTime - lastTime) / static_cast<double>(frequency);
    frameCount++;
    
    double elapsedTime = (currentTime - startFpsTime) / static_cast<double>(frequency) * Constants::MILLISECONDS_PER_SECOND;

    if (elapsedTime >= Constants::MILLISECONDS_PER_SECOND) {
      fps = frameCount;
      frameCount = 0;
      startFpsTime = currentTime;
    }

    lastTime = currentTime;
  }

  double FramesCounter::getDeltaTime() const {
    return deltaTime;
  }

  double FramesCounter::getTotalTime() const {
    Uint64 currentTime = SDL_GetPerformanceCounter();
    return (currentTime - startTime) / static_cast<double>(frequency);
  }

  int FramesCounter::getFPS() const {
    return fps;
  }
}
