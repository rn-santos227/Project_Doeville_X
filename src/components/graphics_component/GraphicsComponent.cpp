#include "GraphicsComponent.h"

#include <chrono>
#include <future>

#include <SDL_image.h>

#include "libraries/constants/Constants.h"

namespace Project::Components {
  using Project::Utilities::LogsManager;
  using Project::Handlers::ResourcesHandler;
  using Project::Handlers::AnimationHandler;

  namespace Constants = Project::Libraries::Constants;

  GraphicsComponent::GraphicsComponent(SDL_Renderer* renderer, LogsManager& logsManager)
  : BaseComponent(logsManager), renderer(renderer), logsManager(logsManager) {
    animationHandler = std::make_unique<AnimationHandler>(renderer, logsManager);
  }

  GraphicsComponent::~GraphicsComponent() {
    destroyTexture();
    textureFuture = std::future<SDL_Texture*>();
    pendingTexturePath.clear();
  }

  void GraphicsComponent::update(float deltaTime) {
    if (animationHandler) {
      animationHandler->update(static_cast<Uint32>(deltaTime * Constants::MILLISECONDS_PER_SECOND));
    }
  }

  void GraphicsComponent::render() {
    if (!texture && textureFuture.valid() &&
        textureFuture.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
      SDL_Texture* loaded = textureFuture.get();
      if (!loaded) {
        logsManager.logError("Failed to load texture: " + pendingTexturePath);
      } else {
        texture = loaded;
        int texW = 0;
        int texH = 0;
        if (SDL_QueryTexture(texture, nullptr, nullptr, &texW, &texH) == 0) {
          destRect.w = texW;
          destRect.h = texH;
        }
      }
      pendingTexturePath.clear();
    }

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

    textureFuture = resourcesHandler.loadTextureAsync(renderer, imagePath);
    pendingTexturePath = imagePath;
    drawShape = false;
    return true;
  }

  void GraphicsComponent::setShape(int width, int height, SDL_Color color) {
    destroyTexture();
    textureFuture = std::future<SDL_Texture*>();
    pendingTexturePath.clear();
    destRect.w = width;
    destRect.h = height;
    shapeColor = color;
    drawShape = true;
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
