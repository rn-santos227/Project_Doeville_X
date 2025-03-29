#include "Animation.h"
#include <SDL_image.h>

namespace Project::Handlers {
  Animation::Animation(SDL_Renderer* renderer, LogsManager& logsManager) : renderer(renderer), logsManager(logsManager) {}

  bool Animation::addFrame(const std::string& imagePath, Uint32 duration) {
    SDL_Surface* surface = IMG_Load(imagePath.c_str());

    if (logsManager.checkAndLogError(!surface, "Failed to load image: " + imagePath + " - " + IMG_GetError())) {
      logsManager.flushLogs();
      return false;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    if (logsManager.checkAndLogError(!texture, "Failed to load texture: " + imagePath + " - " + IMG_GetError())) {
      logsManager.flushLogs();
      return false;
    }

    frames.push_back({texture, duration});
    return true;
  }

  void Animation::setPlaybackMode(PlaybackMode mode) {
    playbackMode = mode;
    switch (mode) {
      case PlaybackMode::ONCE:
        break;

      case PlaybackMode::LOOP:
        break;

      case PlaybackMode::PINGPONG:
        break;

      case PlaybackMode::REVERSE:
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

  void Animation::freeFrames() {
    for (auto& frame : frames) {
      SDL_DestroyTexture(frame.texture);
    }
    frames.clear();
  }
}