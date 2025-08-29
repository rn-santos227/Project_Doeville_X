#ifndef ANIMATION_HANDLER_H
#define ANIMATION_HANDLER_H

#include "Animation.h"

#include <unordered_map>
#include <memory>
#include <string>

#include "utilities/logs_manager/LogsManager.h"

namespace Project::Handlers {
  class AnimationHandler {
    public:
      explicit AnimationHandler(SDL_Renderer* renderer, Project::Utilities::LogsManager& logsManager);
      ~AnimationHandler();

      void addAnimation(const std::string& name, Animation animation);
      void playAnimation(const std::string& name);

      void update(Uint32 deltaTime);

      SDL_Texture* getCurrentFrameTexture();
      bool isAnimationActive() const;
      
    private:
      SDL_Renderer* renderer;
      Project::Utilities::LogsManager& logsManager;

      Animation* currentAnimation = nullptr;
      std::unordered_map<std::string, Animation> animations;
  };
}

#endif
