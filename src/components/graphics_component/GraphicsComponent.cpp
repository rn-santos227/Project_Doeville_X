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
#include "utilities/geometry/GeometryUtils.h"

namespace Project::Components {
  Project::Handlers::CameraHandler* GraphicsComponent::cameraHandler = nullptr;

  using Project::Utilities::LogsManager;
  using Project::Utilities::ColorUtils;
  using Project::Utilities::GeometryUtils;
  using Project::Handlers::ResourcesHandler;
  using Project::Handlers::AnimationHandler;
  using Project::Services::StyleManager;

  namespace Constants = Project::Libraries::Constants;
  namespace Keys = Project::Libraries::Keys;

  GraphicsComponent::GraphicsComponent(SDL_Renderer* renderer, ResourcesHandler* resourcesHandler, LogsManager& logsManager)
    : BaseComponent(logsManager), renderer(renderer), resourcesHandler(resourcesHandler), logsManager(logsManager) {
    animationHandler = std::make_unique<AnimationHandler>(renderer, logsManager);

    shapeVertices.reserve(Constants::INDEX_FOUR);
    shapeIndices.reserve(Constants::INDEX_SIX);
    setupShapeIndices();
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
    if (!texture && textureFuture.valid()) {
      checkAsyncTextureLoad();
    }

    if (animationHandler && animationHandler->isAnimationActive()) {
      animationHandler->update(static_cast<Uint32>(deltaTime * Constants::MILLISECONDS_PER_SECOND));
    }
  }

  void GraphicsComponent::render() {
    if (cameraHandler && !isInCameraView()) {
      return;
    }

    const SDL_Rect renderRect = getRenderRect();
    SDL_Texture* textureToRender = getTextureToRender();

    if (textureToRender) {
      renderTexture(textureToRender, renderRect);
    } else if (drawShape) {
      renderShape(renderRect);
    }
  }

  void GraphicsComponent::build(Project::Utilities::LuaStateWrapper& luaStateWrapper, const std::string& tableName) {
    const std::string imagePath = luaStateWrapper.getTableString(tableName, Keys::TEXTURE_PATH, Constants::EMPTY_STRING);
    const std::string colorHex = luaStateWrapper.getTableString(tableName, Keys::COLOR_HEX, Constants::DEFAULT_SHAPE_COLOR_HEX);
    const int radius = static_cast<int>(luaStateWrapper.getTableNumber(tableName, Keys::RADIUS, 0));
    const int width = static_cast<int>(luaStateWrapper.getTableNumber(tableName, Keys::WIDTH, Constants::DEFAULT_COMPONENT_SIZE));
    const int height = static_cast<int>(luaStateWrapper.getTableNumber(tableName, Keys::HEIGHT, Constants::DEFAULT_COMPONENT_SIZE));
    const Uint8 alpha = static_cast<Uint8>(luaStateWrapper.getTableNumber(tableName, Keys::COLOR_ALPHA, Constants::FULL_ALPHA));
    const bool rotation = luaStateWrapper.getTableBoolean(tableName, Keys::ROTATION, false);

    if (!imagePath.empty() && resourcesHandler) {
      setTexture(*resourcesHandler, imagePath);
    } else {
      const SDL_Color color = ColorUtils::hexToRGB(colorHex, alpha);
      if (radius > 0) {
        setCircle(radius, color);
      } else {
        setShape(width, height, color);
      }
    }

    setRotationEnabled(rotation);
    onAttach();
  }

  void GraphicsComponent::applyStyle() {
    std::istringstream classes(getClass());
    std::string cls;
    
    while (classes >> cls) {
      const std::string selector = "." + cls;
      const Project::Services::Style s = StyleManager::getInstance().getStyle(selector);
      
      if (s.width > 0) destRect.w = s.width;
      if (s.height > 0) destRect.h = s.height;
      
      if (s.background.a != 0) {
        shapeColor = s.background;
        drawShape = true;
      }

      if (s.hasGradient) {
        gradientStart = s.gradientStart;
        gradientEnd = s.gradientEnd;
        gradient = s.gradient;
        useGradient = true;
        drawShape = true;
      }

      if (s.borderColor.a != 0) {
        borderColor = s.borderColor;
      }
      
      if (s.borderWidth > 0) {
        borderWidth = s.borderWidth;
      }

      if (s.opacity != Constants::DEFAULT_WHOLE) {
        const float opacity = s.opacity;
        Uint8 newAlpha;
        if (opacity > Constants::DEFAULT_WHOLE) {
          newAlpha = static_cast<Uint8>(std::min(opacity, static_cast<float>(Constants::FULL_ALPHA)));
        } else {
          newAlpha = static_cast<Uint8>(opacity * Constants::FULL_ALPHA);
        }
        shapeColor.a = newAlpha;
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

  SDL_Rect GraphicsComponent::getRenderRect() const {
    SDL_Rect renderRect = destRect;
    if (cameraHandler) {
      renderRect.x -= cameraHandler->getX();
      renderRect.y -= cameraHandler->getY();
    }
    return renderRect;
  }

  SDL_Texture* GraphicsComponent::getTextureToRender() {
    if (animationHandler && animationHandler->isAnimationActive()) {
      SDL_Texture* animTexture = animationHandler->getCurrentFrameTexture();
      if (animTexture) {
        return animTexture;
      }
    }
    return texture;
  }

  bool GraphicsComponent::isInCameraView() {
    if (!cameraHandler) return true;
    
    const SDL_Rect cullRect = cameraHandler->getCullingRect();
    return SDL_HasIntersection(&destRect, &cullRect);
  }

  void GraphicsComponent::checkAsyncTextureLoad() {
    if (textureFuture.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
      SDL_Texture* loaded = textureFuture.get();
      if (!loaded) {
        logsManager.logError("Failed to load texture: " + pendingTexturePath);
      } else {
        texture = loaded;
        int texW, texH;
        if (SDL_QueryTexture(texture, nullptr, nullptr, &texW, &texH) == 0) {
          destRect.w = texW;
          destRect.h = texH;
        }
      }
      pendingTexturePath.clear();
    }
  }

  void GraphicsComponent::renderTexture(SDL_Texture* textureToRender, const SDL_Rect& renderRect) {
    if (rotationEnabled) {
      SDL_RenderCopyEx(renderer, textureToRender, nullptr, &renderRect, rotation, nullptr, SDL_FLIP_NONE);
    } else {
      SDL_RenderCopy(renderer, textureToRender, nullptr, &renderRect);
    }
  }


  void GraphicsComponent::renderShape(const SDL_Rect& renderRect) {
    if (isCircle) {
      renderCircleShape(renderRect);
    } else {
      renderRectShape(renderRect);
    }
  }

  void GraphicsComponent::renderCircleShape(const SDL_Rect& renderRect) {
    const int centerX = renderRect.x + static_cast<int>(radius);
    const int centerY = renderRect.y + static_cast<int>(radius);
    const int circleRadius = static_cast<int>(radius);

    SDL_SetRenderDrawColor(renderer, shapeColor.r, shapeColor.g, shapeColor.b, shapeColor.a);
    GeometryUtils::renderFilledCircle(renderer, centerX, centerY, circleRadius);

    if (borderWidth > 0) {
      SDL_SetRenderDrawColor(renderer, borderColor.r, borderColor.g, borderColor.b, borderColor.a);
      for (int i = 0; i < borderWidth; ++i) {
        GeometryUtils::renderCircle(renderer, centerX, centerY, circleRadius - i);
      }
    }
  }

  void GraphicsComponent::renderRectShape(const SDL_Rect& renderRect) {
    const bool needsComplexRendering = rotationEnabled || useGradient;
    
    if (needsComplexRendering) {
      renderComplexRect(renderRect);
    } else {
      renderSimpleRect(renderRect);
    }

    if (borderWidth > 0) {
      renderRectBorder(renderRect);
    }
  }

  void GraphicsComponent::renderSimpleRect(const SDL_Rect& renderRect) {
    SDL_SetRenderDrawColor(renderer, shapeColor.r, shapeColor.g, shapeColor.b, shapeColor.a);
    SDL_RenderFillRect(renderer, &renderRect);
  }

  void GraphicsComponent::renderComplexRect(const SDL_Rect& renderRect) {
    updateRotationCache();
    
    const float cx = renderRect.x + renderRect.w * Constants::DEFAULT_HALF;
    const float cy = renderRect.y + renderRect.h * Constants::DEFAULT_HALF;
    
    shapeVertices.clear();
    shapeVertices.resize(Constants::INDEX_FOUR);
    
    const std::array<SDL_FPoint, Constants::INDEX_FOUR> localCorners = {{
      {static_cast<float>(renderRect.x), static_cast<float>(renderRect.y)},
      {static_cast<float>(renderRect.x + renderRect.w), static_cast<float>(renderRect.y)},
      {static_cast<float>(renderRect.x + renderRect.w), static_cast<float>(renderRect.y + renderRect.h)},
      {static_cast<float>(renderRect.x), static_cast<float>(renderRect.y + renderRect.h)}
    }};

    for (int i = 0; i < Constants::INDEX_FOUR; ++i) {
      if (rotationEnabled) {
        const float rx = localCorners[i].x - cx;
        const float ry = localCorners[i].y - cy;
        shapeVertices[i].position.x = rx * cachedCos - ry * cachedSin + cx;
        shapeVertices[i].position.y = rx * cachedSin + ry * cachedCos + cy;
      } else {
        shapeVertices[i].position = localCorners[i];
      }
      
      if (useGradient) {
        if (gradient == Project::Services::GradientType::VERTICAL) {
          shapeVertices[i].color = (i < Constants::INDEX_TWO) ? gradientStart : gradientEnd;
        } else {
          shapeVertices[i].color = (i == 0 || i == Constants::INDEX_THREE) ? gradientStart : gradientEnd;
        }
      } else {
        shapeVertices[i].color = shapeColor;
      }
      
      shapeVertices[i].tex_coord = {0.0f, 0.0f};
    }

    SDL_RenderGeometry(renderer, nullptr, shapeVertices.data(), Constants::INDEX_FOUR, shapeIndices.data(), Constants::INDEX_SIX);
  }

  void GraphicsComponent::renderRectBorder(const SDL_Rect& renderRect) {
    SDL_SetRenderDrawColor(renderer, borderColor.r, borderColor.g, borderColor.b, borderColor.a);
    for (int i = 0; i < borderWidth; ++i) {
      const SDL_Rect borderRect = {
        renderRect.x + i, 
        renderRect.y + i, 
        renderRect.w - 2 * i, 
        renderRect.h - 2 * i
      };
      SDL_RenderDrawRect(renderer, &borderRect);
    }
  }

  void GraphicsComponent::updateRotationCache() {
    if (rotation != lastCachedRotation) {
      const float angleRad = rotation * static_cast<float>(M_PI) / Constants::ANGLE_180_DEG;
      cachedCos = std::cos(angleRad);
      cachedSin = std::sin(angleRad);
      lastCachedRotation = rotation;
    }
  }

  void GraphicsComponent::setupShapeIndices() {
    shapeIndices = {0, 1, 2, 2, 3, 0};
  }
}
