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

  bool GraphicsComponent::setTexture(const std::string& imagePath) {
    freeTexture();

    SDL_Surface* surface = IMG_Load(imagePath.c_str());
    if(logsManager.checkAndLogError(!surface, "Failed to load image: " + imagePath + " - " + IMG_GetError())) {

    }
  }

  void GraphicsComponent::freeTexture() {
    if (texture) {
      SDL_DestroyTexture(texture);
      texture = nullptr;
    }
  }
}