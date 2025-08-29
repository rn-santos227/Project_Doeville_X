#include "GraphicsComponent.h"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <future>

#include <SDL_image.h>

#include "assets/texture_asset/TextureAsset.h"
#include "handlers/camera/Camera.h"
#include "handlers/camera/CameraHandler.h"
#include "libraries/keys/Keys.h"
#include "libraries/constants/Constants.h"
#include "services/styling/StyleManager.h"
#include "utilities/color/ColorUtils.h"
#include "utilities/geometry/GeometryUtils.h"
#include "utilities/profiler/Profiler.h"

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

  GraphicsComponent::GraphicsComponent(
    SDL_Renderer* renderer,
    ResourcesHandler* resourcesHandler,
    Project::Assets::AssetsManager& assetsManager,
    LogsManager& logsManager)
    : BaseComponent(logsManager),
      renderer(renderer),
      resourcesHandler(resourcesHandler),
      assetsManager(assetsManager),
      logsManager(logsManager) {

    animationHandler = std::make_unique<AnimationHandler>(renderer, logsManager);
    shapeVertices.reserve(Constants::INDEX_FOUR);
    data.shapeIndices.reserve(Constants::INDEX_SIX);
    data.verticesDirty = true;
    setupShapeIndices();
  }

  GraphicsComponent::~GraphicsComponent() {
    if (!data.pendingTexturePath.empty()) {
      destroyTexture();
    }
    textureFuture = std::future<SDL_Texture*>();
    data.pendingTexturePath.clear();
    data.assetName.clear();
  }


  std::size_t GraphicsComponent::getTextureMemory(SDL_Texture* tex) {
    if (!tex) return 0u;
    Uint32 format;
    int access, w, h;
    if (SDL_QueryTexture(tex, &format, &access, &w, &h) != 0) return 0u;
    int bpp = SDL_BYTESPERPIXEL(format);
    return static_cast<std::size_t>(w) * static_cast<std::size_t>(h) * static_cast<std::size_t>(bpp);
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

    if (!lodLevels.empty() && cameraHandler) {
      const float camX = cameraHandler->getX();
      const float camY = cameraHandler->getY();
      const float cx = data.destRect.x + data.destRect.w * Constants::DEFAULT_HALF;
      const float cy = data.destRect.y + data.destRect.h * Constants::DEFAULT_HALF;
      const float dx = cx - camX;
      const float dy = cy - camY;
      const float dist2 = dx * dx + dy * dy;
      int newLOD = static_cast<int>(lodLevels.size()) - 1;
      for (int i = 0; i < static_cast<int>(lodLevels.size()); ++i) {
        if (dist2 <= lodLevels[i].distance2) { newLOD = i; break; }
      }
      if (newLOD != activeLOD) {
        std::size_t before = getTextureMemory(texture);
        Project::Utilities::Profiler::getInstance().setMemoryUsage(Constants::PROFILE_LOD, before);
        if (resourcesHandler) {
          setTexture(*resourcesHandler, lodLevels[newLOD].assetPath);
        }
        activeLOD = newLOD;
        lodSwapPending = true;
      }
    }

    if (animationHandler && animationHandler->isAnimationActive()) {
      animationHandler->update(static_cast<Uint32>(deltaTime * Constants::MILLISECONDS_PER_SECOND));
    }
  }

  void GraphicsComponent::render() {
    if (cameraHandler && !isInCameraView()) {
      return;
    }

    const SDL_FRect renderRect = getRenderRect();
    SDL_Texture* textureToRender = getTextureToRender();

    if (textureToRender) {
      renderTexture(textureToRender, renderRect);
    } else if (data.drawShape) {
      renderShape(renderRect);
    }
  }

  void GraphicsComponent::build(Project::Utilities::LuaStateWrapper& luaStateWrapper, const std::string& tableName) {
    const std::string assetName = luaStateWrapper.getTableString(tableName, Keys::ASSET_NAME, Constants::EMPTY_STRING);
    const std::string colorHex = luaStateWrapper.getTableString(tableName, Keys::COLOR_HEX, Constants::DEFAULT_SHAPE_COLOR_HEX);
    const int radius = static_cast<int>(luaStateWrapper.getTableNumber(tableName, Keys::RADIUS, 0));
    const int width = static_cast<int>(luaStateWrapper.getTableNumber(tableName, Keys::WIDTH, Constants::DEFAULT_COMPONENT_SIZE));
    const int height = static_cast<int>(luaStateWrapper.getTableNumber(tableName, Keys::HEIGHT, Constants::DEFAULT_COMPONENT_SIZE));
    const Uint8 alpha = static_cast<Uint8>(luaStateWrapper.getTableNumber(tableName, Keys::COLOR_ALPHA, Constants::FULL_ALPHA));
    const bool rotation = luaStateWrapper.getTableBoolean(tableName, Keys::ROTATION, false);
    const bool occ = luaStateWrapper.getTableBoolean(tableName, Keys::OCCLUDER, false);

    const SDL_Color color = ColorUtils::hexToRGB(colorHex, alpha);
    lua_State* L = luaStateWrapper.get();
    lua_getglobal(L, tableName.c_str());
    if (lua_istable(L, -1)) {
      lua_getfield(L, -1, Keys::LODS);
      if (lua_istable(L, -1)) {
        lua_pushnil(L);
        while (lua_next(L, -2)) {
          if (lua_istable(L, -1)) {
            std::string path;
            float dist = 0.0f;
            lua_getfield(L, -1, Keys::ASSET_PATH);
            if (lua_isstring(L, -1)) path = lua_tostring(L, -1);
            lua_pop(L, 1);
            lua_getfield(L, -1, Keys::DISTANCE);
            if (lua_isnumber(L, -1)) dist = static_cast<float>(lua_tonumber(L, -1));
            lua_pop(L, 1);
            if (!path.empty()) {
              lodLevels.push_back({path, dist * dist});
            }
          }
          lua_pop(L, 1);
        }
        lua_pop(L, 1);
        std::sort(lodLevels.begin(), lodLevels.end(), [](const LODLevel& a, const LODLevel& b){ return a.distance2 < b.distance2; });
        if (!lodLevels.empty() && resourcesHandler) {
          setTexture(*resourcesHandler, lodLevels.front().assetPath);
          activeLOD = 0;
        }
      } else {
        lua_pop(L, 1);
      }
      lua_pop(L, 1);
    } else {
      lua_pop(L, 1);
    }

    if (lodLevels.empty()) {
      if (!assetName.empty()) {
        auto* baseAsset = assetsManager.getAsset(assetName);
        if (auto* textureAsset = dynamic_cast<Project::Assets::TextureAsset*>(baseAsset)) {
          texture = textureAsset->getTexture();
          SDL_SetTextureBlendMode(texture, data.blendMode);
          data.destRect.w = textureAsset->getWidth();
          data.destRect.h = textureAsset->getHeight();
          data.drawShape = false;
          data.assetName = assetName;
        } else {
          logsManager.logError("Asset not found or not a texture: " + assetName + ", using shape instead");
          if (radius > 0) {
            setCircle(radius, color);
          } else {
            setShape(width, height, color);
          }
        }
      } else {
        if (radius > 0) {
          setCircle(radius, color);
        } else {
          setShape(width, height, color);
        }
      }
    }

    setRotationEnabled(rotation);
    setOccluder(occ);
    onAttach();
  }

  void GraphicsComponent::applyStyle() {
    std::istringstream classes(getClass());
    std::string cls;
    while (classes >> cls) {
      const std::string selector = "." + cls;
      const Project::Services::Style s = StyleManager::getInstance().getStyle(selector);
      
      if (s.width > 0) {
        if (data.destRect.w != s.width) data.verticesDirty = true;
        data.destRect.w = s.width;
      }
      if (s.height > 0) {
        if (data.destRect.h != s.height) data.verticesDirty = true;
        data.destRect.h = s.height;
      }
      
      if (s.background.a != 0) {
        data.shapeColor = s.background;
        data.drawShape = true;
      }

      if (s.hasGradient) {
        data.gradientStart = s.gradientStart;
        data.gradientEnd = s.gradientEnd;
        if (data.gradient != s.gradient) data.verticesDirty = true;
        data.gradient = s.gradient;
        data.useGradient = true;
        data.drawShape = true;
        data.verticesDirty = true;
      }

      if (s.borderColor.a != 0) {
        data.borderColor = s.borderColor;
      }
      
      if (s.borderWidth > 0) {
        data.borderWidth = s.borderWidth;
      }

      if (s.opacity != Constants::DEFAULT_WHOLE) {
        const float opacity = s.opacity;
        Uint8 newAlpha;
        if (opacity > Constants::DEFAULT_WHOLE) {
          newAlpha = static_cast<Uint8>(std::min(opacity, static_cast<float>(Constants::FULL_ALPHA)));
        } else {
          newAlpha = static_cast<Uint8>(opacity * Constants::FULL_ALPHA);
        }
        data.shapeColor.a = newAlpha;
      }
    }
  }

  bool GraphicsComponent::setTexture(ResourcesHandler& resourcesHandler, const std::string& imagePath) {
    destroyTexture();

    textureFuture = resourcesHandler.loadTextureAsync(renderer, imagePath);
    data.pendingTexturePath = imagePath;
    data.assetName.clear();
    data.drawShape = false;
    data.srcRect = SDL_Rect{0,0,0,0};
    return true;
  }

  void GraphicsComponent::setShape(int width, int height, SDL_Color color) {
    if (!data.pendingTexturePath.empty()) {
      destroyTexture();
    }
    
    textureFuture = std::future<SDL_Texture*>();
    data.pendingTexturePath.clear();
    data.assetName.clear();
    
    if (data.destRect.w != width || data.destRect.h != height) data.verticesDirty = true;

    data.destRect.w = width;
    data.destRect.h = height;
    data.shapeColor = color;
    data.drawShape = true;
    data.isCircle = false;
    data.radius = 0;
    updateBoundingBox();
  }

  void GraphicsComponent::setCircle(int r, SDL_Color color) {
    destroyTexture();
    textureFuture = std::future<SDL_Texture*>();
    data.pendingTexturePath.clear();
    if (data.destRect.w != r * Constants::CIRCLE_DIAMETER_MULTIPLIER ||
        data.destRect.h != r * Constants::CIRCLE_DIAMETER_MULTIPLIER)
      data.verticesDirty = true;

    data.destRect.w = r * Constants::CIRCLE_DIAMETER_MULTIPLIER;
    data.destRect.h = r * Constants::CIRCLE_DIAMETER_MULTIPLIER;
    data.shapeColor = color;
    data.drawShape = true;
    data.isCircle = true;
    data.radius = r;
    updateBoundingBox();
  }

  void GraphicsComponent::setColor(SDL_Color color) {
    data.shapeColor = color;
  }

  void GraphicsComponent::setPosition(float x, float y, float width, float height) {
    data.destRect.x = x;
    data.destRect.y = y;
    if (data.destRect.w != width || data.destRect.h != height) data.verticesDirty = true;
    data.destRect.w = width;
    data.destRect.h = height;
    updateBoundingBox();
  }

  void GraphicsComponent::setEntityPosition(float x, float y) {
    setPosition(x, y, data.destRect.w, data.destRect.h);
  }

  SDL_FRect GraphicsComponent::getRect() const {
    return data.destRect;
  }

  SDL_Texture* GraphicsComponent::getBatchTexture() {
    return getTextureToRender();
  }

  bool GraphicsComponent::isInFrustum(const Project::Handlers::Camera& cam) const {
    SDL_FPoint pts[Constants::INDEX_FOUR] = {
      {boundingBox.x, boundingBox.y},
      {boundingBox.x + boundingBox.w, boundingBox.y},
      {boundingBox.x + boundingBox.w, boundingBox.y + boundingBox.h},
      {boundingBox.x, boundingBox.y + boundingBox.h}
    };
    for (const auto& plane : cam.planes) {
      bool outside = true;
      for (const auto& p : pts) {
        if (plane.test(p.x, p.y)) { outside = false; break; }
      }
      if (outside) return false;
    }
    return true;
  }

  bool GraphicsComponent::isVisible(
    const Project::Handlers::Camera& cam,
    const std::vector<SDL_FRect>& occluders,
    int frame) const {
    if (lastVisibilityFrame == frame) return lastVisibilityResult;
    lastVisibilityFrame = frame;
    bool visible = isInFrustum(cam);
    if (visible && !occluder) {
      for (const auto& occ : occluders) {
        if (occ.x <= boundingBox.x && occ.y <= boundingBox.y &&
            occ.x + occ.w >= boundingBox.x + boundingBox.w &&
            occ.y + occ.h >= boundingBox.y + boundingBox.h) {
          visible = false;
          break;
        }
      }
    }
    lastVisibilityResult = visible;
    return visible;
  }

  SDL_FRect GraphicsComponent::getRenderRect() const {
    SDL_FRect renderRect = data.destRect;

    if (cameraHandler) {
      const float zoom = cameraHandler->getZoom();
      const float camX = std::floor(cameraHandler->getX());
      const float camY = std::floor(cameraHandler->getY());
      renderRect.x = std::round((data.destRect.x - camX) * zoom);
      renderRect.y = std::round((data.destRect.y - camY) * zoom);
      renderRect.w = std::round(data.destRect.w * zoom);
      renderRect.h = std::round(data.destRect.h * zoom);
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
    const SDL_FRect cullRect = cameraHandler->getCullingRect();
    SDL_FRect worldRect = data.destRect;
    return SDL_HasIntersectionF(&worldRect, &cullRect);
  }

  void GraphicsComponent::checkAsyncTextureLoad() {
    if (textureFuture.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
      SDL_Texture* loaded = textureFuture.get();
      if (!loaded) {
        logsManager.logError("Failed to load texture: " + data.pendingTexturePath);
      } else {
        texture = loaded;
        SDL_SetTextureBlendMode(texture, data.blendMode);
        if (resourcesHandler) {
          SDL_Rect region = resourcesHandler->getTextureRegion(renderer, data.pendingTexturePath);
          data.srcRect = region;
          if (region.w > 0 && region.h > 0) {
            if (data.destRect.w != region.w || data.destRect.h != region.h) data.verticesDirty = true;
            data.destRect.w = region.w;
            data.destRect.h = region.h;
          } else {
            int texW, texH;
            if (SDL_QueryTexture(texture, nullptr, nullptr, &texW, &texH) == 0) {
              if (data.destRect.w != texW || data.destRect.h != texH) data.verticesDirty = true;
              data.destRect.w = texW;
              data.destRect.h = texH;
            }
          }
        }
      }
      updateBoundingBox();
      data.pendingTexturePath.clear();
      if (lodSwapPending) {
        std::size_t after = getTextureMemory(texture);
        Project::Utilities::Profiler::getInstance().setMemoryUsage("lod_after", after);
        lodSwapPending = false;
      }
    }
  }

  void GraphicsComponent::renderTexture(SDL_Texture* textureToRender, const SDL_FRect& renderRect) {
    const SDL_Rect* src = (data.srcRect.w > 0 && data.srcRect.h > 0) ? &data.srcRect : nullptr;
    if (data.rotationEnabled) {
      SDL_RenderCopyExF(renderer, textureToRender, src, &renderRect, data.rotation, nullptr, SDL_FLIP_NONE);
    } else {
      SDL_RenderCopyF(renderer, textureToRender, src, &renderRect);
    }
  }


  void GraphicsComponent::renderShape(const SDL_FRect& renderRect) {
    if (data.isCircle) {
      renderCircleShape(renderRect);
    } else {
      renderRectShape(renderRect);
    }
  }

  void GraphicsComponent::renderCircleShape(const SDL_FRect& renderRect) {
    const float zoom = cameraHandler ? cameraHandler->getZoom() : Constants::DEFAULT_CAMERA_ZOOM_STEP;
    const float radius = data.radius * zoom;
    const int centerX = static_cast<int>(renderRect.x + radius);
    const int centerY = static_cast<int>(renderRect.y + radius);
    const int circleRadius = static_cast<int>(radius);

    SDL_SetRenderDrawColor(renderer, data.shapeColor.r, data.shapeColor.g, data.shapeColor.b, data.shapeColor.a);
    GeometryUtils::renderFilledCircle(renderer, centerX, centerY, circleRadius);

    int scaledBorder = static_cast<int>(std::round(data.borderWidth * zoom));
    if (scaledBorder > 0) {
      SDL_SetRenderDrawColor(renderer, data.borderColor.r, data.borderColor.g, data.borderColor.b, data.borderColor.a);
      for (int i = 0; i < scaledBorder; ++i) {
        GeometryUtils::renderCircle(renderer, centerX, centerY, circleRadius - i);
      }
    }
  }

  void GraphicsComponent::renderRectShape(const SDL_FRect& renderRect) {
    const bool needsComplexRendering = data.rotationEnabled || data.useGradient;
    
    if (needsComplexRendering) {
      renderComplexRect(renderRect);
    } else {
      renderSimpleRect(renderRect);
    }

    if (data.borderWidth > 0) {
      renderRectBorder(renderRect);
    }
  }

  void GraphicsComponent::renderSimpleRect(const SDL_FRect& renderRect) {
    SDL_SetRenderDrawColor(renderer, data.shapeColor.r, data.shapeColor.g, data.shapeColor.b, data.shapeColor.a);
    SDL_RenderFillRectF(renderer, &renderRect);
  }

  void GraphicsComponent::renderComplexRect(const SDL_FRect& renderRect) {
    updateRotationCache();
    
    if (data.verticesDirty) {
      const float cx = renderRect.x + renderRect.w * Constants::DEFAULT_HALF;
      const float cy = renderRect.y + renderRect.h * Constants::DEFAULT_HALF;

      shapeVertices.clear();
      shapeVertices.resize(Constants::INDEX_FOUR);

      const std::array<SDL_FPoint, Constants::INDEX_FOUR> localCorners = {{
        {renderRect.x, renderRect.y},
        {renderRect.x + renderRect.w, renderRect.y},
        {renderRect.x + renderRect.w, renderRect.y + renderRect.h},
        {renderRect.x, renderRect.y + renderRect.h}
      }};

      for (int i = 0; i < Constants::INDEX_FOUR; ++i) {
        if (data.rotationEnabled) {
          const float rx = localCorners[i].x - cx;
          const float ry = localCorners[i].y - cy;
          shapeVertices[i].position.x = rx * cachedCos - ry * cachedSin + cx;
          shapeVertices[i].position.y = rx * cachedSin + ry * cachedCos + cy;
        } else {
          shapeVertices[i].position = localCorners[i];
        }

        if (data.useGradient) {
          if (data.gradient == Project::Services::GradientType::VERTICAL) {
            shapeVertices[i].color = (i < Constants::INDEX_TWO) ? data.gradientStart : data.gradientEnd;
          } else {
            shapeVertices[i].color = (i == 0 || i == Constants::INDEX_THREE) ? data.gradientStart : data.gradientEnd;
          }
        } else {
          shapeVertices[i].color = data.shapeColor;
        }
      }
      
      data.verticesDirty = false;
    }
    SDL_RenderGeometry(renderer, nullptr, shapeVertices.data(), Constants::INDEX_FOUR, data.shapeIndices.data(), Constants::INDEX_SIX);
  }

  void GraphicsComponent::renderRectBorder(const SDL_FRect& renderRect) {
    SDL_SetRenderDrawColor(renderer, data.borderColor.r, data.borderColor.g, data.borderColor.b, data.borderColor.a);
    const float zoom = cameraHandler ? cameraHandler->getZoom() : Constants::DEFAULT_CAMERA_ZOOM_STEP;
    int scaledBorder = static_cast<int>(std::round(data.borderWidth * zoom));
    for (int i = 0; i < scaledBorder; ++i) {
      const SDL_FRect borderRect = {
        renderRect.x + static_cast<float>(i),
        renderRect.y + static_cast<float>(i),
        renderRect.w - Constants::INDEX_TWO * static_cast<float>(i),
        renderRect.h - Constants::INDEX_TWO * static_cast<float>(i)
      };
      SDL_RenderDrawRectF(renderer, &borderRect);
    }
  }

  void GraphicsComponent::setupShapeIndices() {
    data.shapeIndices.assign(
      Constants::RECTANGLE_INDICES.begin(),
      Constants::RECTANGLE_INDICES.end()
    );
  }

  void GraphicsComponent::updateRotationCache() {
    if (data.rotation != lastCachedRotation) {
      const float angleRad = data.rotation * static_cast<float>(M_PI) / Constants::ANGLE_180_DEG;
      cachedCos = std::cos(angleRad);
      cachedSin = std::sin(angleRad);
      lastCachedRotation = data.rotation;
    }
  }

  void GraphicsComponent::updateBoundingBox() {
    boundingBox.x = static_cast<float>(data.destRect.x);
    boundingBox.y = static_cast<float>(data.destRect.y);
    boundingBox.w = static_cast<float>(data.destRect.w);
    boundingBox.h = static_cast<float>(data.destRect.h);
  }
}
