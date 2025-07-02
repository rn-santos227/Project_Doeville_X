#include "GraphicsComponent.h"

#include <SDL_image.h>

namespace Project::Components {
  GraphicsComponent::GraphicsComponent(SDL_Renderer* renderer, LogsManager& logsManager)
  : BaseComponent(logsManager), renderer(renderer), logsManager(logsManager) {
    animationHandler = std::make_unique<AnimationHandler>(renderer, logsManager);
  }

  GraphicsComponent::~GraphicsComponent() {
    destroyTexture();
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
    } else if (drawShape) {
      SDL_SetRenderDrawColor(renderer, shapeColor.r, shapeColor.g, shapeColor.b, shapeColor.a);
      SDL_RenderFillRect(renderer, &destRect);
    }  
  }

  bool GraphicsComponent::setTexture(ResourcesHandler& resourcesHandler, const std::string& imagePath) {
    destroyTexture();

    auto futureTex = resourcesHandler.loadTextureAsync(renderer, imagePath);
    SDL_Texture* loaded = futureTex.get();
    if (logsManager.checkAndLogError(!loaded, "Failed to load texture: " + imagePath)) {
      return false;
    }

    texture = loaded;

    int texW = 0;
    int texH = 0;
    if (SDL_QueryTexture(texture, nullptr, nullptr, &texW, &texH) == 0) {
      destRect.w = texW;
      destRect.h = texH;
    }
    
    return true;
  }

  void GraphicsComponent::setShape(int width, int height, SDL_Color color) {
    destroyTexture();
    destRect.w = width;
  }

  void GraphicsComponent::setPosition(int x, int y, int width, int height) {
    destRect.x = x;
    destRect.y = y;
    destRect.w = width;
    destRect.h = height;
  }

  void GraphicsComponent::setEntityPosition(int x, int y) {
    setPosition(x, y, destRect.w, destRect.h);
  }
}
