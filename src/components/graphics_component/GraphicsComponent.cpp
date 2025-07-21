#include "GraphicsComponent.h"

#include <chrono>
#include <cmath>
#include <future>

#include <SDL_image.h>

#include "handlers/camera/CameraHandler.h"
#include "libraries/keys/Keys.h"
#include "libraries/constants/Constants.h"
#include "services/styling/StyleManager.h"
#include "utilities/color/ColorUtils.h"

namespace Project::Components {
  Project::Handlers::CameraHandler* GraphicsComponent::cameraHandler = nullptr;

  using Project::Utilities::LogsManager;
  using Project::Utilities::ColorUtils;
  using Project::Handlers::ResourcesHandler;
  using Project::Handlers::AnimationHandler;
  using Project::Services::StyleManager;

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

  void GraphicsComponent::setCameraHandler(Project::Handlers::CameraHandler* handler) {
    cameraHandler = handler;
  }

  Project::Handlers::CameraHandler* GraphicsComponent::getCameraHandler() {
    return cameraHandler;
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

    if (cameraHandler) {
      SDL_Rect cullRect = cameraHandler->getCullingRect();
      if (!SDL_HasIntersection(&destRect, &cullRect)) {
        return;
      }
    }
    SDL_Texture* textureToRender = texture;

    if (animationHandler && animationHandler->isAnimationActive()) {
      SDL_Texture* animTexture = animationHandler->getCurrentFrameTexture();
      if (animTexture) {
        textureToRender = animTexture;
      }
    }

    SDL_Rect renderRect = destRect;
    if (cameraHandler) {
      renderRect.x -= cameraHandler->getX();
      renderRect.y -= cameraHandler->getY();
    }

    if (textureToRender) {
      if (rotationEnabled) {
        SDL_RenderCopyEx(renderer, textureToRender, nullptr, &renderRect, rotation, nullptr, SDL_FLIP_NONE);
      } else {
        SDL_RenderCopy(renderer, textureToRender, nullptr, &renderRect);
      }
    } else if (drawShape) {
      if (isCircle) {
        SDL_SetRenderDrawColor(renderer, shapeColor.r, shapeColor.g, shapeColor.b, shapeColor.a);
        for (int w = 0; w < radius * Constants::DEFAULT_DOUBLE; ++w) {
          for (int h = 0; h < radius * Constants::DEFAULT_DOUBLE; ++h) {
            int dx = radius - w;
            int dy = radius - h;
            if ((dx * dx + dy * dy) <= (radius * radius)) {
              SDL_RenderDrawPoint(renderer, renderRect.x + dx + radius, renderRect.y + dy + radius);
            }
          }
        }
      } else {
        bool complex = rotationEnabled || useGradient;
        if (complex) {
          float angleRad = rotation * static_cast<float>(M_PI) / Constants::ANGLE_180_DEG;
          float cosA = std::cos(angleRad);
          float sinA = std::sin(angleRad);
          float cx = renderRect.x + renderRect.w * Constants::DEFAULT_HALF;
          float cy = renderRect.y + renderRect.h * Constants::DEFAULT_HALF;
          SDL_FPoint corners[Constants::INDEX_FOUR] = {
            {static_cast<float>(renderRect.x), static_cast<float>(renderRect.y)},
            {static_cast<float>(renderRect.x + renderRect.w), static_cast<float>(renderRect.y)},
            {static_cast<float>(renderRect.x + renderRect.w), static_cast<float>(renderRect.y + renderRect.h)},
            {static_cast<float>(renderRect.x), static_cast<float>(renderRect.y + renderRect.h)}
          };
          SDL_Vertex verts[Constants::INDEX_FOUR];
          for (int i = 0; i < Constants::INDEX_FOUR; ++i) {
            float rx = corners[i].x - cx;
            float ry = corners[i].y - cy;
            verts[i].position.x = rotationEnabled ? rx * cosA - ry * sinA + cx : corners[i].x;
            verts[i].position.y = rotationEnabled ? rx * sinA + ry * cosA + cy : corners[i].y;
            SDL_Color col = shapeColor;
            if (useGradient) {
              if (gradient == Project::Services::GradientType::VERTICAL) {
                col = (i < 2) ? gradientStart : gradientEnd;
              } else {
                col = (i == 0 || i == 3) ? gradientStart : gradientEnd;
              }
            }
            verts[i].color = col;
            verts[i].tex_coord = {0.0f, 0.0f};
          }
          int indices[Constants::INDEX_SIX] = {
            Constants::INDEX_ZERO,
            Constants::INDEX_ONE,
            Constants::INDEX_TWO,
            Constants::INDEX_TWO,
            Constants::INDEX_THREE,
            Constants::INDEX_ZERO
          };
          SDL_RenderGeometry(renderer, nullptr, verts, Constants::INDEX_FOUR, indices, Constants::INDEX_SIX);
        } else {
          SDL_SetRenderDrawColor(renderer, shapeColor.r, shapeColor.g, shapeColor.b, shapeColor.a);
          SDL_RenderFillRect(renderer, &renderRect);
        }
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

    bool rotation = luaStateWrapper.getTableBoolean(tableName, Keys::ROTATION, false);
    setRotationEnabled(rotation);

    onAttach();
  }

  void GraphicsComponent::applyStyle() {
    std::istringstream classes(getClass());
    std::string cls;
    while (classes >> cls) {
      std::string selector = "." + cls;
      Project::Services::Style s = Project::Services::StyleManager::getInstance().getStyle(selector);
      if (s.width > 0) destRect.w = s.width;
      if (s.height > 0) destRect.h = s.height;
      if (s.background.a != 0) {
        shapeColor = s.background;
        drawShape = true;
      }

      if (s.opacity != Constants::DEFAULT_WHOLE) {
        float opacity = s.opacity;
        Uint8 alpha = 0;
        if (opacity > Constants::DEFAULT_WHOLE) {
          alpha = static_cast<Uint8>(std::min(opacity, static_cast<float>(Project::Libraries::Constants::FULL_ALPHA)));
        } else {
          alpha = static_cast<Uint8>(opacity * Project::Libraries::Constants::FULL_ALPHA);
        }
        shapeColor.a = alpha;
      }
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
