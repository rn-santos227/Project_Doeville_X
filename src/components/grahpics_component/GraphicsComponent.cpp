#include "GraphicsComponent.h"
#include <SDL_image.h>

namespace Project::Components {
  GraphicsComponent::GraphicsComponent(SDL_Renderer* renderer, LogsManager& logsManager)
  : BaseComponent(logsManager), renderer(renderer), logsManager(logsManager) {
    animationHandler = std::make_unique<AnimationHandler>(renderer, logsManager);
  }
}