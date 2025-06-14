#include "Animation.h"
#include <SDL_image.h>

namespace Project::Handlers {
  Animation::Animation(SDL_Renderer* renderer, LogsManager& logsManager, ResourcesHandler& resourcesHandler)
    : renderer(renderer), logsManager(logsManager), resourcesHandler(resourcesHandler) {}

  Animation::~Animation() {
    freeFrames();
    logsManager.logMessage("Animation instance destroyed and textures freed.");
  }

  bool Animation::addFrame(const std::string& imagePath, Uint32 duration) {
    auto futureTex = resourcesHandler.loadTextureAsync(renderer, imagePath);
    if (logsManager.checkAndLogError(!texture, "Failed to load image: " + imagePath)) {
      return false;
    }

    frames.push_back({texture, duration});
    return true;
  }

  void Animation::setPlaybackMode(PlaybackMode mode) {
    playbackMode = mode;
    switch (mode) {
      case PlaybackMode::ONCE:
        looping = false;
        break;

      case PlaybackMode::LOOP:
        looping = true;
        break;

      case PlaybackMode::PINGPONG:
        looping = true;
        forward = true;
        break;

      case PlaybackMode::REVERSE:
        looping = true;
        currentFrameIndex = frames.size() - 1;
        break;

      default:
        logsManager.logWarning("Invalid playback mode");
        break;
    }
  }

  void Animation::play(bool reset) {
    playing = true;
    finished = false;

    if (reset) {
      currentFrameIndex = 0;
      elapsedTime = 0;
      forward = true;
    }
  }

  void Animation::stop() {
    playing = false;
  }

  void Animation::reset() {
    currentFrameIndex = 0;
    elapsedTime = 0;
    forward = true;
    finished = false;
  }

  void Animation::update(Uint32 deltaTime) {
    if (!playing || frames.empty() || finished) return;
    
    elapsedTime += deltaTime;
    if (elapsedTime >= frames[currentFrameIndex].duration) {
      elapsedTime = 0;

      switch (playbackMode) {
        case PlaybackMode::LOOP:
          currentFrameIndex = (currentFrameIndex + 1) % frames.size();
          break;

        case PlaybackMode::ONCE:
          if (currentFrameIndex < frames.size() - 1) {
            currentFrameIndex++;
          } else {
            finished = true;
          }
          break;

        case PlaybackMode::PINGPONG:
          if (forward) {
            if (currentFrameIndex < frames.size() - 1) {
              currentFrameIndex++;
            } else {
              forward = false;
              currentFrameIndex--;
            }
          } else {
            if (currentFrameIndex > 0) {
              currentFrameIndex--;
            } else {
              forward = true;
              if (!looping) finished = true;
            }
          }
          break;

        case PlaybackMode::REVERSE:
          if (currentFrameIndex > 0) {
            currentFrameIndex--;
          } else {
            if (looping) {
              currentFrameIndex = frames.size() - 1;
            } else {
              finished = true;
            }
          }
          break;
      }
    }
  }

  SDL_Texture* Animation::getCurrentFrameTexture() const {
    if (frames.empty()) return nullptr;
    return frames[currentFrameIndex].texture;
  }

  void Animation::freeFrames() {
    frames.clear();
  }
}
