#include "GraphicsComponent.h"

#include <chrono>
#include <future>

#include <SDL_image.h>

#include "libraries/keys/Keys.h"
#include "libraries/constants/Constants.h"
#include "utilities/color/ColorUtils.h"

namespace Project::Components {
  using Project::Utilities::LogsManager;
  using Project::Utilities::ColorUtils;
  using Project::Handlers::ResourcesHandler;
  using Project::Handlers::AnimationHandler;

  namespace Constants = Project::Libraries::Constants;
  namespace Keys = Project::Libraries::Keys;

  GraphicsComponent::GraphicsComponent(SDL_Renderer* renderer, ResourcesHandler* resourcesHandler, LogsManager& logsManager)
    : BaseComponent(logsManager), renderer(renderer), resourcesHandler(resourcesHandler), logsManager(logsManager) {
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
      if (isCircle) {
        for (int w = 0; w < radius * 2; ++w) {
          for (int h = 0; h < radius * 2; ++h) {
            int dx = radius - w;
            int dy = radius - h;
            if ((dx * dx + dy * dy) <= (radius * radius)) {
              SDL_RenderDrawPoint(renderer, destRect.x + dx + radius, destRect.y + dy + radius);
            }
          }
        }
      } else {
        SDL_RenderFillRect(renderer, &destRect);
      }
    }
  }

  void GraphicsComponent::build(Project::Utilities::LuaStateWrapper& luaStateWrapper, const std::string& tableName) {
    namespace Keys = Project::Libraries::Keys;
    namespace Constants = Project::Libraries::Constants;

    std::string imagePath = luaStateWrapper.getTableString(tableName, Keys::TEXTURE_PATH, "");
    if (!imagePath.empty() && resourcesHandler) {
      setTexture(*resourcesHandler, imagePath);
    } else {
      int radius = static_cast<int>(luaStateWrapper.getTableNumber(tableName, Keys::RADIUS, 0));
      std::string colorHex = luaStateWrapper.getTableString(tableName, Keys::COLOR_HEX, Constants::DEFAULT_SHAPE_COLOR_HEX);
      Uint8 alpha = static_cast<Uint8>(luaStateWrapper.getTableNumber(tableName, Keys::COLOR_ALPHA, Constants::FULL_ALPHA));
      SDL_Color color = Project::Utilities::ColorUtils::hexToRGB(colorHex, alpha);
      if (radius > 0) {
        setCircle(radius, color);
      } else {
        int width = static_cast<int>(luaStateWrapper.getTableNumber(tableName, Keys::WIDTH, Constants::DEFAULT_COMPONENT_SIZE));
        int height = static_cast<int>(luaStateWrapper.getTableNumber(tableName, Keys::HEIGHT, Constants::DEFAULT_COMPONENT_SIZE));
        setShape(width, height, color);
      }
    }

    bool active = luaStateWrapper.getTableBoolean(tableName, Keys::ACTIVE, true);
    setActive(active);
    onAttach();
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
    isCircle = false;
    radius = 0;
  }

  void GraphicsComponent::setCircle(int r, SDL_Color color) {
    destroyTexture();
    textureFuture = std::future<SDL_Texture*>();
    pendingTexturePath.clear();
    destRect.w = r * 2;
    destRect.h = r * 2;
    shapeColor = color;
    drawShape = true;
    isCircle = true;
    radius = r;
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
