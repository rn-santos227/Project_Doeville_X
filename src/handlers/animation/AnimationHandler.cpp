#include "AnimationHandler.h"

namespace Project::Handlers {
  AnimationHandler::AnimationHandler(SDL_Renderer* renderer, LogsManager& logsManager)
  : renderer(renderer), logsManager(logsManager) {}

  AnimationHandler::~AnimationHandler() = default;

  void AnimationHandler::addAnimation(const std::string& name, Animation animation) {
    animations.emplace(name, std::move(animation));
  }
}