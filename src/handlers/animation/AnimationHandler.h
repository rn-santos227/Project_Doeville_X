#ifndef ANIMATION_HANDLER_H
#define ANIMATION_HANDLER_H

#include "Animation.h"

#include <unordered_map>
#include <memory>
#include <string>

#include "utilities/logs_manager/LogsManager.h"

using namespace Project::Utilities;
namespace Project::Handlers {
  class AnimationHandler {
    public:
      explicit AnimationHandler(SDL_Renderer* renderer, LogsManager& logsManager);
      ~AnimationHandler();

      void addAnimation(const std::string& name, Animation animation);
      void playAnimation(const std::string& name);

      void update(Uint32 deltaTime);
      void render();

    private:
      std::unordered_map<std::string, Animation> animations;
  };
}

#endif