#include "BoundingBoxComponent.h"
#include "SurfaceType.h"
#include "SurfaceTypeResolver.h"

#include <algorithm>
#include <cmath>
#include <limits>

#include "handlers/camera/CameraHandler.h"
#include "entities/Entity.h"
#include "entities/EntitiesManager.h"
#include "libraries/categories/Categories.h"
#include "libraries/constants/FloatConstants.h"
#include "libraries/keys/Keys.h"

namespace Project::Components {
   Project::Handlers::CameraHandler* BoundingBoxComponent::cameraHandler = nullptr;

  using Project::Utilities::LogsManager;
  using Project::Utilities::GeometryUtils;
  using Project::Handlers::KeyHandler;
  using Project::Components::SurfaceType;
  using Project::Components::SurfaceTypeResolver;

  namespace Constants = Project::Libraries::Constants;
  namespace Keys = Project::Libraries::Keys;
  namespace Surfaces = Project::Libraries::Categories::Surfaces;
  
  BoundingBoxComponent::BoundingBoxComponent(LogsManager& logsManager, SDL_Renderer* renderer, KeyHandler* keyHandler, SDL_Color debugColor)
    : BaseComponent(logsManager), PositionableComponent(), renderer(renderer), keyHandler(keyHandler) {
      
      data.boxes.reserve(Constants::INDEX_FOUR);
      data.circles.reserve(Constants::INDEX_TWO);
      data.orientedBoxes.reserve(Constants::INDEX_FOUR);
      
      worldBoxes.reserve(Constants::INDEX_FOUR);
      worldCircles.reserve(Constants::INDEX_TWO);
  }

  void BoundingBoxComponent::setCameraHandler(Project::Handlers::CameraHandler* handler) {
    cameraHandler = handler;
  }

  void BoundingBoxComponent::update(float /*deltaTime*/) {}

  void BoundingBoxComponent::render() {
    if (!renderer || !keyHandler || !keyHandler->isGameDebugMode()) return;
    
    const float camX = cameraHandler ? std::floor(cameraHandler->getX()) : 0.0f;
    const float camY = cameraHandler ? std::floor(cameraHandler->getY()) : 0.0f;
    const float zoom = cameraHandler ? cameraHandler->getZoom() : 1.0f;

    SDL_SetRenderDrawColor(renderer, data.debugColor.r, data.debugColor.g, data.debugColor.b, data.debugColor.a);
    if (worldBoxesDirty) {
      updateWorldBoxes();
      worldBoxesDirty = false;
    }

    if (data.rotationEnabled) {
      for (const auto& box : data.orientedBoxes) {
        for (int i = 0; i < Constants::INDEX_FOUR; ++i) {
          const SDL_FPoint& p1 = box.corners[i];
          const SDL_FPoint& p2 = box.corners[(i + 1) % Constants::INDEX_FOUR];

          SDL_RenderDrawLineF(
            renderer,
            std::round((p1.x - camX) * zoom),
            std::round((p1.y - camY) * zoom),
            std::round((p2.x - camX) * zoom),
            std::round((p2.y - camY) * zoom)
          );
        }
      }
    } else {
      for (const auto& rect : worldBoxes) {

        SDL_FRect r {
          std::round((rect.x - camX) * zoom),
          std::round((rect.y - camY) * zoom),
          std::round(rect.w * zoom),
          std::round(rect.h * zoom)
        };
        SDL_RenderDrawRectF(renderer, &r);
      }
    }

    static bool sinCosTableInitialized = false;
    static std::array<float, 360> sinTable, cosTable;
    if (!sinCosTableInitialized) {
      for (int angle = 0; angle < 360; ++angle) {
        const float rad = angle * Constants::DEG_TO_RAD;
        sinTable[angle] = std::sin(rad);
        cosTable[angle] = std::cos(rad);
      }
      sinCosTableInitialized = true;
    }

    for (const auto& circle : worldCircles) {
      for (int angle = 0; angle < Constants::ANGLE_360_DEG; ++angle) {
        float rad = angle * Constants::DEG_TO_RAD;
        float px = std::round((circle.x + circle.r * std::cos(rad) - camX) * zoom);
        float py = std::round((circle.y + circle.r * std::sin(rad) - camY) * zoom);
        SDL_RenderDrawPointF(renderer, px, py);
      }
    }
  }

  void BoundingBoxComponent::build(Project::Utilities::LuaStateWrapper& luaStateWrapper, const std::string& tableName) {
    lua_State* L = luaStateWrapper.get();
    lua_getglobal(L, tableName.c_str());
    if (lua_istable(L, -1)) {
      lua_getfield(L, -1, Keys::BOXES);
      if (lua_istable(L, -1)) {
        lua_pushnil(L);
        while (lua_next(L, -2)) {
          if (lua_istable(L, -1)) {
            float x = 0.0f, y = 0.0f, w = 0.0f, h = 0.0f, r = 0.0f;
            lua_getfield(L, -1, Keys::X);
            if (lua_isnumber(L, -1)) x = static_cast<float>(lua_tonumber(L, -1));
            lua_pop(L, 1);
            lua_getfield(L, -1, Keys::Y);
            if (lua_isnumber(L, -1)) y = static_cast<float>(lua_tonumber(L, -1));
            lua_pop(L, 1);
            lua_getfield(L, -1, Keys::W);
            if (lua_isnumber(L, -1)) w = static_cast<float>(lua_tonumber(L, -1));
            lua_pop(L, 1);
            lua_getfield(L, -1, Keys::H);
            if (lua_isnumber(L, -1)) h = static_cast<float>(lua_tonumber(L, -1));
            lua_pop(L, 1);
            lua_getfield(L, -1, Keys::RADIUS);
            if (lua_isnumber(L, -1)) r = static_cast<float>(lua_tonumber(L, -1));
            lua_pop(L, 1);
            if (r > 0.0f) {
              addCircle(x, y, r);
            } else {
              SDL_FRect rect{x, y, w, h};
              addBox(rect);
            }
          }
          lua_pop(L, 1);
        }
        lua_pop(L, 1);
      }
      lua_pop(L, 1);
    } else {
      lua_pop(L, 1);
    }

    std::string surfaceStr = luaStateWrapper.getTableString(tableName, Keys::SURFACE, Surfaces::REST);
    setSurfaceType(SurfaceTypeResolver::resolve(surfaceStr));

    bool solidValue = luaStateWrapper.getTableBoolean(tableName, Keys::SOLID, false);
    setSolid(solidValue);

    float fric = static_cast<float>(luaStateWrapper.getTableNumber(tableName, Keys::FRICTION, Project::Libraries::Constants::DEFAULT_FRICTION));
    setFriction(fric);

    float rest = static_cast<float>(luaStateWrapper.getTableNumber(tableName, Keys::RESTITUTION, Project::Libraries::Constants::DEFAULT_BOUNCE_FACTOR));
    setRestitution(rest);

    bool rot = luaStateWrapper.getTableBoolean(tableName, Keys::ROTATION, false);
    setRotationEnabled(rot);

    bool proxy = luaStateWrapper.getTableBoolean(tableName, Keys::USE_PROXY, false);
    setUseProxy(proxy);
    if (!useProxy && (!data.polygons.empty() || !data.capsules.empty())) {
      useProxy = true;
    }

    markDirty();
  }

  void BoundingBoxComponent::addBox(const SDL_FRect& rect) {
    data.boxes.push_back(rect);
    updateWorldBoxes();
  }

  void BoundingBoxComponent::addCircle(float x, float y, float r) {
    data.circles.push_back({x, y, r});
    updateWorldBoxes();
  }

  void BoundingBoxComponent::addPolygon(const std::vector<SDL_FPoint>& points) {
    data.polygons.push_back({points});
    updateWorldBoxes();
  }

  void BoundingBoxComponent::addCapsule(const Project::Utilities::Capsule& cap) {
    data.capsules.push_back(cap);
    updateWorldBoxes();
  }

  void BoundingBoxComponent::setIgnoredEntities(const std::vector<std::string>& names) {
    ignoredEntities.clear();
    for (const auto& n : names) {
      ignoredEntities.insert(n);
    }
  }

  bool BoundingBoxComponent::isIgnoring(const std::string& name) const {
    return ignoredEntities.find(name) != ignoredEntities.end();
  }

  void BoundingBoxComponent::clearShapes() {
    data.boxes.clear();
    data.circles.clear();
    data.orientedBoxes.clear();
    data.polygons.clear();
    data.capsules.clear();

    worldBoxes.clear();
    worldCircles.clear();
    worldPolygons.clear();
    worldCapsules.clear();
  }

  SDL_FRect BoundingBoxComponent::getProxyAABB() const {
    SDL_FRect bounds{0.f,0.f,0.f,0.f};
    bool hasBounds = false;

    const auto& rects = getBoxes();
    for (const auto& r : rects) {
      if (!hasBounds) { bounds = r; hasBounds = true; }
      else {
        const float left = std::min(bounds.x, r.x);
        const float top = std::min(bounds.y, r.y);
        const float right = std::max(bounds.x + bounds.w, r.x + r.w);
        const float bottom = std::max(bounds.y + bounds.h, r.y + r.h);
        bounds = {left, top, right - left, bottom - top};
      }
    }

    const auto& circles = getCircles();
    for (const auto& c : circles) {
      SDL_FRect r{
        c.x - c.r, c.y - c.r,
        c.r * Project::Libraries::Constants::CIRCLE_DIAMETER_MULTIPLIER,
        c.r * Project::Libraries::Constants::CIRCLE_DIAMETER_MULTIPLIER}
        ;
      if (!hasBounds) { bounds = r; hasBounds = true; }
      else {
        const float left = std::min(bounds.x, r.x);
        const float top = std::min(bounds.y, r.y);
        const float right = std::max(bounds.x + bounds.w, r.x + r.w);
        const float bottom = std::max(bounds.y + bounds.h, r.y + r.h);
        bounds = {left, top, right - left, bottom - top};
      }
    }

    const auto& polys = getPolygons();
    for (const auto& p : polys) {
      SDL_FRect r = Project::Utilities::GeometryUtils::polygonBounds(p);
      if (!hasBounds) { bounds = r; hasBounds = true; }
      else {
        const float left = std::min(bounds.x, r.x);
        const float top = std::min(bounds.y, r.y);
        const float right = std::max(bounds.x + bounds.w, r.x + r.w);
        const float bottom = std::max(bounds.y + bounds.h, r.y + r.h);
        bounds = {left, top, right - left, bottom - top};
      }
    }

    const auto& caps = getCapsules();
    for (const auto& c : caps) {
      SDL_FRect r = Project::Utilities::GeometryUtils::capsuleBounds(c);
      if (!hasBounds) { bounds = r; hasBounds = true; }
      else {
        const float left = std::min(bounds.x, r.x);
        const float top = std::min(bounds.y, r.y);
        const float right = std::max(bounds.x + bounds.w, r.x + r.w);
        const float bottom = std::max(bounds.y + bounds.h, r.y + r.h);
        bounds = {left, top, right - left, bottom - top};
      }
    }

    return bounds;
  }

  bool BoundingBoxComponent::isInteractive() const {
    if (data.solid) return true;
    auto surface = getSurfaceType();
    return surface == SurfaceType::DESTROY_ON_HIT || 
           surface == SurfaceType::TRIGGER_EVENT ||
           surface == SurfaceType::GHOST_PASS;
  }

  bool BoundingBoxComponent::handleSurfaceInteraction(
    SurfaceType surface, Project::Entities::Entity* target,
    const SDL_FPoint& offset, float bounce, float fric,
    float& velocityX, float& velocityY) {
    
    if (surface == SurfaceType::GHOST_PASS) {
      return false;
    }

    const float oneMinusFric = Constants::DEFAULT_WHOLE - fric;
    
    switch (surface) {
      case SurfaceType::SLIDE:
        if (std::abs(offset.x) >= std::abs(offset.y)) {
          velocityX = 0.0f;
        } else {
          velocityY = 0.0f;
        }
        velocityX *= oneMinusFric;
        velocityY *= oneMinusFric;
        break;
        
      case SurfaceType::STICK:
      case SurfaceType::REST:
        velocityX = 0.0f;
        velocityY = 0.0f;
        break;
        
      case SurfaceType::DESTROY_ON_HIT:
        if (target) {
           auto* manager = target->getEntitiesManager();
          if (manager) {
            manager->removeEntity(target->getEntityID());
          }
        }
        break;
        
      case SurfaceType::TRIGGER_EVENT:
        if (target) {
          target->getLuaStateWrapper().callFunctionIfExists(Keys::LUA_ON_TRIGGER);
        }
        break;
        
      default:
        velocityX = -velocityX * bounce * oneMinusFric;
        velocityY = -velocityY * bounce * oneMinusFric;
        break;
    }
    return true;
  }

  void BoundingBoxComponent::setEntityPosition(float x, float y) {
    if (entityX != x || entityY != y) {
      entityX = x;
      entityY = y;
      updateWorldBoxes();
      worldBoxesDirty = false;
    }
  }

  void BoundingBoxComponent::setEntityRotation(float angle) {
    if (data.rotation != angle) {
      data.rotation = angle;
      lastCachedRotation = std::numeric_limits<float>::quiet_NaN();
      updateWorldBoxes();
      worldBoxesDirty = false;
    }
  }

 const std::vector<SDL_FRect>& BoundingBoxComponent::getBoxes() const {
    if (worldBoxesDirty) {
      const_cast<BoundingBoxComponent*>(this)->updateWorldBoxes();
      worldBoxesDirty = false;
    }
    ensureUpdated();
    return worldBoxes;
  }

  const std::vector<Project::Utilities::Circle>& BoundingBoxComponent::getCircles() const {
    if (worldBoxesDirty) {
      const_cast<BoundingBoxComponent*>(this)->updateWorldBoxes();
      worldBoxesDirty = false;
    }
    ensureUpdated();
    return worldCircles;
  }

  const std::vector<Project::Utilities::OrientedBox>& BoundingBoxComponent::getOrientedBoxes() const {
    if (worldBoxesDirty) {
      const_cast<BoundingBoxComponent*>(this)->updateWorldBoxes();
      worldBoxesDirty = false;
    }
    ensureUpdated();
    return data.orientedBoxes;
  }

  void BoundingBoxComponent::ensureUpdated() const {
    if (worldBoxesDirty) {
      const_cast<BoundingBoxComponent*>(this)->updateWorldBoxes();
      worldBoxesDirty = false;
    }
  }

  void BoundingBoxComponent::markDirty() {
    worldBoxesDirty = true;
  }

  void BoundingBoxComponent::updateWorldBoxes() {
    if (worldBoxes.size() != data.boxes.size()) {
      worldBoxes.resize(data.boxes.size());
      data.orientedBoxes.resize(data.boxes.size());
    }

    if (worldCircles.size() != data.circles.size()) {
      worldCircles.resize(data.circles.size());
    }

    if (worldPolygons.size() != data.polygons.size()) {
      worldPolygons.resize(data.polygons.size());
    }

    if (worldCapsules.size() != data.capsules.size()) {
      worldCapsules.resize(data.capsules.size());
    }

    float cosA = 1.0f, sinA = 0.0f;
    if (data.rotationEnabled && !data.boxes.empty()) {
      if (data.rotation != lastCachedRotation) {
        const float angleRad = data.rotation * static_cast<float>(M_PI) / Constants::ANGLE_180_DEG;
        cachedCos = std::cos(angleRad);
        cachedSin = std::sin(angleRad);
        lastCachedRotation = data.rotation;
      }
      cosA = cachedCos;
      sinA = cachedSin;
    }

    for (size_t i = 0; i < data.boxes.size(); ++i) {
      if (data.rotationEnabled) {
        const float cx = data.boxes[i].x + data.boxes[i].w * Constants::DEFAULT_HALF;
        const float cy = data.boxes[i].y + data.boxes[i].h * Constants::DEFAULT_HALF;
        
        const std::array<SDL_FPoint, 4> localCorners = {{
          {data.boxes[i].x - cx, data.boxes[i].y - cy},
          {data.boxes[i].x + data.boxes[i].w - cx, data.boxes[i].y - cy},
          {data.boxes[i].x + data.boxes[i].w - cx, data.boxes[i].y + data.boxes[i].h - cy},
          {data.boxes[i].x - cx, data.boxes[i].y + data.boxes[i].h - cy}
        }};
        
        float minX = std::numeric_limits<float>::max();
        float minY = std::numeric_limits<float>::max();
        float maxX = std::numeric_limits<float>::lowest();
        float maxY = std::numeric_limits<float>::lowest();
        
        for (int j = 0; j < Constants::INDEX_FOUR; ++j) {
          const float rx = localCorners[j].x;
          const float ry = localCorners[j].y;
          const float newX = rx * cosA - ry * sinA + cx + entityX;
          const float newY = rx * sinA + ry * cosA + cy + entityY;

          data.orientedBoxes[i].corners[j] = {newX, newY};
          minX = std::min(minX, newX);
          minY = std::min(minY, newY);
          maxX = std::max(maxX, newX);
          maxY = std::max(maxY, newY);
        }
        
        worldBoxes[i] = {minX, minY, maxX - minX, maxY - minY};
      } else {
        worldBoxes[i] = {data.boxes[i].x + entityX, data.boxes[i].y + entityY, data.boxes[i].w, data.boxes[i].h};

        const float x = data.boxes[i].x + entityX;
        const float y = data.boxes[i].y + entityY;
        const float w = data.boxes[i].w;
        const float h = data.boxes[i].h;
        
        data.orientedBoxes[i].corners[0] = {x, y};
        data.orientedBoxes[i].corners[1] = {x + w, y};
        data.orientedBoxes[i].corners[2] = {x + w, y + h};
        data.orientedBoxes[i].corners[3] = {x, y + h};
      }
    }

    for (size_t i = 0; i < data.circles.size(); ++i) {
      worldCircles[i].x = data.circles[i].x + entityX;
      worldCircles[i].y = data.circles[i].y + entityY;
      worldCircles[i].r = data.circles[i].r;
    }

    for (size_t i = 0; i < data.polygons.size(); ++i) {
      worldPolygons[i].vertices.resize(data.polygons[i].vertices.size());
      if (data.rotationEnabled) {
        SDL_FRect bounds = GeometryUtils::polygonBounds(data.polygons[i]);
        const float cx = bounds.x + bounds.w * Constants::DEFAULT_HALF;
        const float cy = bounds.y + bounds.h * Constants::DEFAULT_HALF;
        for (size_t j = 0; j < data.polygons[i].vertices.size(); ++j) {
          const float rx = data.polygons[i].vertices[j].x - cx;
          const float ry = data.polygons[i].vertices[j].y - cy;
          worldPolygons[i].vertices[j].x = rx * cosA - ry * sinA + cx + entityX;
          worldPolygons[i].vertices[j].y = rx * sinA + ry * cosA + cy + entityY;
        }
      } else {
        for (size_t j = 0; j < data.polygons[i].vertices.size(); ++j) {
          worldPolygons[i].vertices[j].x = data.polygons[i].vertices[j].x + entityX;
          worldPolygons[i].vertices[j].y = data.polygons[i].vertices[j].y + entityY;
        }
      }
    }

    for (size_t i = 0; i < data.capsules.size(); ++i) {
      if (data.rotationEnabled) {
        const auto& cap = data.capsules[i];
        const float cx = (cap.start.x + cap.end.x) * Constants::DEFAULT_HALF;
        const float cy = (cap.start.y + cap.end.y) * Constants::DEFAULT_HALF;
        float sx = cap.start.x - cx;
        float sy = cap.start.y - cy;
        float ex = cap.end.x - cx;
        float ey = cap.end.y - cy;
        worldCapsules[i].start.x = sx * cosA - sy * sinA + cx + entityX;
        worldCapsules[i].start.y = sx * sinA + sy * cosA + cy + entityY;
        worldCapsules[i].end.x   = ex * cosA - ey * sinA + cx + entityX;
        worldCapsules[i].end.y   = ex * sinA + ey * cosA + cy + entityY;
        worldCapsules[i].r = cap.r;
      } else {
        worldCapsules[i].start.x = data.capsules[i].start.x + entityX;
        worldCapsules[i].start.y = data.capsules[i].start.y + entityY;
        worldCapsules[i].end.x = data.capsules[i].end.x + entityX;
        worldCapsules[i].end.y = data.capsules[i].end.y + entityY;
        worldCapsules[i].r = data.capsules[i].r;
      }
    }
  }
}
