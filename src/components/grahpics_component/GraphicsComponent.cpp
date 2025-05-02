#include "GraphicsComponent.h"
#include <SDL_image.h>

namespace Project::Components {
  GraphicsComponent::GraphicsComponent(SDL_Renderer* renderer, LogsManager& logsManager)
  : BaseComponent(logsManager), renderer(renderer), logsManager(logsManager) {
    animationHandler = std::make_unique<AnimationHandler>(renderer, logsManager);
  }

  GraphicsComponent::~GraphicsComponent() {
    freeTexture();
  }

  void GraphicsComponent::update(float deltaTime) {
    if (animationHandler) {
      animationHandler->update(static_cast<Uint32>(deltaTime * 1000));
    }
  }

  void GraphicsComponent::render() {
    SDL_Texture* textureToRender = texture;

    if (animationHandler && animationHandler->isAnimationActive()) {

    }
  }

  void GraphicsComponent::freeTexture() {
    if (texture) {
      SDL_DestroyTexture(texture);
      texture = nullptr;
    }
  }
}