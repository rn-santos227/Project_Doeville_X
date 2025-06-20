#ifndef ANIMATION_H
#define ANIMATION_H

#include "PlaybackMode.h"

#include <vector>
#include <string>
#include <SDL.h>

#include "handlers/resources/ResourcesHandler.h"
#include "utilities/logs_manager/LogsManager.h"

using namespace Project::Handlers;
using namespace Project::Utilities;

namespace Project::Handlers {
  struct AnimationFrame {
    SDL_Texture* texture;
    Uint32 duration;
  };

  class Animation {
  public:
    explicit Animation(SDL_Renderer* renderer, LogsManager& logsManager, ResourcesHandler& resourcesHandler);
    ~Animation();

    bool addFrame(const std::string& imagePath, Uint32 duration);
    
    void setPlaybackMode(PlaybackMode mode);
    void play(bool reset = true);
    void stop();
    void reset();
    void update(Uint32 deltaTime);
    
    SDL_Texture* getCurrentFrameTexture() const;
    bool isFinished() const { return finished; }
    bool isPlaying() const { return playing; }
    
  private:
    LogsManager& logsManager;
    ResourcesHandler& resourcesHandler;
    
    SDL_Renderer* renderer;
    std::vector<AnimationFrame> frames;
    PlaybackMode playbackMode = PlaybackMode::LOOP;

    Uint32 elapsedTime = 0;
    size_t currentFrameIndex = 0;
    
    bool forward = true;
    bool playing = false;
    bool finished = false;
    bool looping = true;

    void freeFrames();
  };
}

#endif
