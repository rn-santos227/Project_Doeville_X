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
      SDL_Texture* animTexture = animationHandler->getCurrentFrameTexture();
      if (animTexture) {
        textureToRender = animTexture;
      }
    }

    if (textureToRender) {
      SDL_RenderCopy(renderer, textureToRender, nullptr, &destRect);
    }
  }

  bool GraphicsComponent::setTexture(ResourcesHandler& resourcesHandler, const std::string& imagePath) {
    freeTexture();

    SDL_Texture* loaded = resourcesHandler.loadTexture(renderer, imagePath);
    if (logsManager.checkAndLogError(!loaded, "Failed to load texture: " + imagePath)) {
      return false;
    }

    texture = loaded;
    return true;
  }

  void GraphicsComponent::setPosition(int x, int y, int width, int height) {
    destRect.x = x;
    destRect.y = y;
    destRect.w = width;
    destRect.h = height;
  }

  void GraphicsComponent::freeTexture() {
    if (texture) {
      SDL_DestroyTexture(texture);
      texture = nullptr;
    }
  }
}
