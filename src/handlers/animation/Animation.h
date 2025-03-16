#ifndef ANIMATION_H
#define ANIMATION_H

#include "PlaybackMode.h"

#include <vector>
#include <string>
#include <SDL.h>

namespace Project::Handlers {
  struct AnimationFrame {
    SDL_Texture* texture;
    Uint32 duration;
  };

  class Animation {
  public:
    explicit Animation(SDL_Renderer* renderer);
    ~Animation();

    bool addFrame(const std::string& imagePath, Uint32 duration);
    
  private:
    SDL_Renderer* renderer;
    std::vector<AnimationFrame> frames;
    PlaybackMode playbackMode = PlaybackMode::LOOP;

    Uint32 elapsedTime = 0;
    size_t currentFrameIndex = 0;
    bool forward = true;
    bool playing = false;
    bool finished = false;

    void freeFrames();
  };
}

#endif