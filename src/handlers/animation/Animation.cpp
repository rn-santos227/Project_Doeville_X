#include "Animation.h"

#include <chrono>
#include <future>

#include <SDL_image.h>

#include "libraries/constants/NumericConstants.h"

namespace Project::Handlers {
  using Project::Utilities::LogsManager;
  using Project::Handlers::ResourcesHandler;

  Animation::Animation(SDL_Renderer* renderer, LogsManager& logsManager, ResourcesHandler& resourcesHandler)
    : renderer(renderer), logsManager(logsManager), resourcesHandler(resourcesHandler) {
    frames.reserve(Project::Libraries::Constants::INT_TEN);
  }

  Animation::Animation(Animation&& other) noexcept
    : logsManager(other.logsManager),
      resourcesHandler(other.resourcesHandler),
      renderer(other.renderer),
      frames(std::move(other.frames)),
      playbackMode(other.playbackMode),
      elapsedTime(other.elapsedTime),
      currentFrameIndex(other.currentFrameIndex),
      forward(other.forward),
      playing(other.playing),
      finished(other.finished),
      looping(other.looping) {
    other.renderer = nullptr;
    other.frames.clear();
  }

  Animation& Animation::operator=(Animation&& other) noexcept {
    if (this != &other) {
      freeFrames();
      renderer = other.renderer;
      frames = std::move(other.frames);
      playbackMode = other.playbackMode;
      elapsedTime = other.elapsedTime;
      currentFrameIndex = other.currentFrameIndex;
      forward = other.forward;
      playing = other.playing;
      finished = other.finished;
      looping = other.looping;

      other.renderer = nullptr;
      other.frames.clear();
    }
    return *this;
  }

  Animation::~Animation() {
    freeFrames();
    logsManager.logMessage("Animation instance destroyed and textures freed.");
  }

  bool Animation::addFrame(const std::string& imagePath, Uint32 duration) {
    AnimationFrame frame;
    frame.future = resourcesHandler.loadTextureAsync(renderer, imagePath);
    frame.path = imagePath;
    frame.duration = duration;
    frames.push_back(std::move(frame));
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

  SDL_Texture* Animation::getCurrentFrameTexture() {
    if (frames.empty()) return nullptr;
    
    AnimationFrame& frame = frames[currentFrameIndex];
    if (!frame.texture && frame.future.valid() &&
        frame.future.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
      SDL_Texture* tex = frame.future.get();
      if (!tex) {
        logsManager.logError("Failed to load animation frame: " + frame.path);
      } else {
        frame.texture = tex;
      }
    }

    return frame.texture;
  }

  void Animation::freeFrames() {
    frames.clear();
  }
}
