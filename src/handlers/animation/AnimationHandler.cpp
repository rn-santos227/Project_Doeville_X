#include "AnimationHandler.h"

namespace Project::Handlers {
  using Project::Utilities::LogsManager;

  AnimationHandler::AnimationHandler(SDL_Renderer* renderer, LogsManager& logsManager)
  : renderer(renderer), logsManager(logsManager) {}

  AnimationHandler::~AnimationHandler() = default;

  void AnimationHandler::addAnimation(const std::string& name, Animation animation) {
    animations.emplace(name, std::move(animation));
  }

  void AnimationHandler::playAnimation(const std::string& name) {
    auto it = animations.find(name);
    if (it != animations.end()) {
      currentAnimation = &it->second;
      currentAnimation->play(true);
    }
  }

  void AnimationHandler::update(Uint32 deltaTime) {
    if (currentAnimation) {
      currentAnimation->update(deltaTime);
    }
  }

  SDL_Texture* AnimationHandler::getCurrentFrameTexture() {
    if (currentAnimation) {
      return currentAnimation->getCurrentFrameTexture();
    }
    return nullptr;
  }

  bool AnimationHandler::isAnimationActive() const {
    return currentAnimation && currentAnimation->isPlaying();
  }
}
