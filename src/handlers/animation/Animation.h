#ifndef ANIMATION_H
#define ANIMATION_H

#include "PlaybackMode.h"

#include <future>
#include <string>
#include <vector>

#include <SDL.h>

#include "handlers/resources/ResourcesHandler.h"
#include "utilities/logs_manager/LogsManager.h"

namespace Project::Handlers {
  struct AnimationFrame {
    SDL_Texture* texture = nullptr;
    std::future<SDL_Texture*> future;
    std::string path;
    Uint32 duration;
  };

  class Animation {
  public:
    explicit Animation(SDL_Renderer* renderer, Project::Utilities::LogsManager& logsManager, Project::Handlers::ResourcesHandler& resourcesHandler);
    ~Animation();

    Animation(const Animation&) = delete;
    Animation& operator=(const Animation&) = delete;
    Animation(Animation&& other) noexcept;
    Animation& operator=(Animation&& other) noexcept;

    bool addFrame(const std::string& imagePath, Uint32 duration);
    
    void setPlaybackMode(PlaybackMode mode);
    void play(bool reset = true);
    void stop();
    void reset();
    void update(Uint32 deltaTime);
    
    SDL_Texture* getCurrentFrameTexture();
    bool isFinished() const { return finished; }
    bool isPlaying() const { return playing; }
    
  private:
    Project::Utilities::LogsManager& logsManager;
    Project::Handlers::ResourcesHandler& resourcesHandler;

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
