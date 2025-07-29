#include "BoundingBoxComponent.h"
#include "SurfaceType.h"
#include "SurfaceTypeResolver.h"

#include <cmath>
#include <limits>

#include "handlers/camera/CameraHandler.h"
#include "entities/Entity.h"
#include "entities/EntitiesManager.h"
#include "libraries/categories/Categories.h"
#include "libraries/constants/Constants.h"
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
    : BaseComponent(logsManager), PositionableComponent(), renderer(renderer), keyHandler(keyHandler), debugColor(debugColor) {
      
      boxes.reserve(Constants::INDEX_FOUR);
      worldBoxes.reserve(Constants::INDEX_FOUR);
      circles.reserve(Constants::INDEX_TWO);
      worldCircles.reserve(Constants::INDEX_TWO);
      orientedBoxes.reserve(Constants::INDEX_FOUR);
  }

  void BoundingBoxComponent::setCameraHandler(Project::Handlers::CameraHandler* handler) {
    cameraHandler = handler;
  }

  void BoundingBoxComponent::update(float /*deltaTime*/) {}

  void BoundingBoxComponent::render() {
    if (!renderer || !keyHandler || !keyHandler->isGameDebugMode()) return;
    
    const int camX = cameraHandler ? cameraHandler->getX() : 0;
    const int camY = cameraHandler ? cameraHandler->getY() : 0;

    SDL_SetRenderDrawColor(renderer, debugColor.r, debugColor.g, debugColor.b, debugColor.a);
    if (worldBoxesDirty) {
      updateWorldBoxes();
      worldBoxesDirty = false;
    }

    if (rotationEnabled) {
      for (const auto& box : orientedBoxes) {
        for (int i = 0; i < Constants::INDEX_FOUR; ++i) {
          const SDL_FPoint& p1 = box.corners[i];
          const SDL_FPoint& p2 = box.corners[(i + 1) % Constants::INDEX_FOUR];
          SDL_RenderDrawLine(renderer,
            static_cast<int>(p1.x - camX), static_cast<int>(p1.y - camY),
            static_cast<int>(p2.x - camX), static_cast<int>(p2.y - camY));
        }
      }
    } else {
      for (const auto& rect : worldBoxes) {
        SDL_Rect r = {rect.x - camX, rect.y - camY, rect.w, rect.h};
        SDL_RenderDrawRect(renderer, &r);
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
        int px = static_cast<int>(circle.x + circle.r * std::cos(rad) - camX);
        int py = static_cast<int>(circle.y + circle.r * std::sin(rad) - camY);
        SDL_RenderDrawPoint(renderer, px, py);
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
            int x = 0, y = 0, w = 0, h = 0, r = 0;
            lua_getfield(L, -1, Keys::X);
            if (lua_isnumber(L, -1)) x = static_cast<int>(lua_tonumber(L, -1));
            lua_pop(L, 1);
            lua_getfield(L, -1, Keys::Y);
            if (lua_isnumber(L, -1)) y = static_cast<int>(lua_tonumber(L, -1));
            lua_pop(L, 1);
            lua_getfield(L, -1, Keys::W);
            if (lua_isnumber(L, -1)) w = static_cast<int>(lua_tonumber(L, -1));
            lua_pop(L, 1);
            lua_getfield(L, -1, Keys::H);
            if (lua_isnumber(L, -1)) h = static_cast<int>(lua_tonumber(L, -1));
            lua_pop(L, 1);
            lua_getfield(L, -1, Keys::RADIUS);
            if (lua_isnumber(L, -1)) r = static_cast<int>(lua_tonumber(L, -1));
            lua_pop(L, 1);
            if (r > 0) {
              addCircle(x, y, r);
            } else {
              SDL_Rect rect{x, y, w, h};
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

    markDirty();
  }

  void BoundingBoxComponent::addBox(const SDL_Rect& rect) {
    boxes.push_back(rect);
    updateWorldBoxes();
  }

  void BoundingBoxComponent::addCircle(int x, int y, int r) {
    circles.push_back({x, y, r});
    updateWorldBoxes();
  }

  void BoundingBoxComponent::addPolygon(const std::vector<SDL_FPoint>& points) {
    polygons.push_back({points});
    updateWorldBoxes();
  }

  void BoundingBoxComponent::addCapsule(const Project::Utilities::Capsule& cap) {
    capsules.push_back(cap);
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
    boxes.clear();
    worldBoxes.clear();
    circles.clear();
    worldCircles.clear();
    orientedBoxes.clear();
    worldPolygons.clear();
    capsules.clear();
    worldCapsules.clear();
  }

  void BoundingBoxComponent::setSolid(bool solidEnabled) {
    solid = solidEnabled;
  }

  bool BoundingBoxComponent::isSolid() const {
    return solid;
  }

  bool BoundingBoxComponent::isInteractive() const {
    if (solid) return true;
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

  void BoundingBoxComponent::setEntityPosition(int x, int y) {
     if (entityX != x || entityY != y) {
      entityX = x;
      entityY = y;
      markDirty();
    }
  }

  void BoundingBoxComponent::setEntityRotation(float angle) {
    if (rotation != angle) {
      rotation = angle;
      lastCachedRotation = std::numeric_limits<float>::quiet_NaN();
      markDirty();
    }
  }

  const std::vector<SDL_Rect>& BoundingBoxComponent::getBoxes() const {
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
    return orientedBoxes;
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
    if (worldBoxes.size() != boxes.size()) {
      worldBoxes.resize(boxes.size());
      orientedBoxes.resize(boxes.size());
    }

    if (worldCircles.size() != circles.size()) {
      worldCircles.resize(circles.size());
    }

    if (worldPolygons.size() != polygons.size()) {
      worldPolygons.resize(polygons.size());
    }

    if (worldCapsules.size() != capsules.size()) {
      worldCapsules.resize(capsules.size());
    }

    float cosA = 1.0f, sinA = 0.0f;
    if (rotationEnabled && !boxes.empty()) {
      if (rotation != lastCachedRotation) {
        const float angleRad = rotation * static_cast<float>(M_PI) / Constants::ANGLE_180_DEG;
        cachedCos = std::cos(angleRad);
        cachedSin = std::sin(angleRad);
        lastCachedRotation = rotation;
      }
      cosA = cachedCos;
      sinA = cachedSin;
    }

    for (size_t i = 0; i < boxes.size(); ++i) {
      if (rotationEnabled) {
        const float cx = boxes[i].x + boxes[i].w * Constants::DEFAULT_HALF;
        const float cy = boxes[i].y + boxes[i].h * Constants::DEFAULT_HALF;
        
        const std::array<SDL_FPoint, 4> localCorners = {{
          {static_cast<float>(boxes[i].x) - cx, static_cast<float>(boxes[i].y) - cy},
          {static_cast<float>(boxes[i].x + boxes[i].w) - cx, static_cast<float>(boxes[i].y) - cy},
          {static_cast<float>(boxes[i].x + boxes[i].w) - cx, static_cast<float>(boxes[i].y + boxes[i].h) - cy},
          {static_cast<float>(boxes[i].x) - cx, static_cast<float>(boxes[i].y + boxes[i].h) - cy}
        }};
        
        float minX = std::numeric_limits<float>::max();
        float minY = std::numeric_limits<float>::max();
        float maxX = std::numeric_limits<float>::lowest();
        float maxY = std::numeric_limits<float>::lowest();
        
        for (int j = 0; j < Constants::INDEX_FOUR; ++j) {
          const float rx = localCorners[j].x;
          const float ry = localCorners[j].y;
          const float newX = rx * cosA - ry * sinA + cx + static_cast<float>(entityX);
          const float newY = rx * sinA + ry * cosA + cy + static_cast<float>(entityY);
          
          orientedBoxes[i].corners[j] = {newX, newY};
          minX = std::min(minX, newX);
          minY = std::min(minY, newY);
          maxX = std::max(maxX, newX);
          maxY = std::max(maxY, newY);
        }
        
        worldBoxes[i] = {
          static_cast<int>(minX),
          static_cast<int>(minY),
          static_cast<int>(maxX - minX),
          static_cast<int>(maxY - minY)
        };
      } else {
        worldBoxes[i] = {
          static_cast<int>(boxes[i].x + entityX),
          static_cast<int>(boxes[i].y + entityY),
          boxes[i].w,
          boxes[i].h
        };
        
        const float x = static_cast<float>(worldBoxes[i].x);
        const float y = static_cast<float>(worldBoxes[i].y);
        const float w = static_cast<float>(worldBoxes[i].w);
        const float h = static_cast<float>(worldBoxes[i].h);
        
        orientedBoxes[i].corners[0] = {x, y};
        orientedBoxes[i].corners[1] = {x + w, y};
        orientedBoxes[i].corners[2] = {x + w, y + h};
        orientedBoxes[i].corners[3] = {x, y + h};
      }
    }

    for (size_t i = 0; i < circles.size(); ++i) {
      worldCircles[i].x = circles[i].x + entityX;
      worldCircles[i].y = circles[i].y + entityY;
      worldCircles[i].r = circles[i].r;
    }

    for (size_t i = 0; i < polygons.size(); ++i) {
      worldPolygons[i].vertices.resize(polygons[i].vertices.size());
      for (size_t j = 0; j < polygons[i].vertices.size(); ++j) {
        worldPolygons[i].vertices[j].x = polygons[i].vertices[j].x + entityX;
        worldPolygons[i].vertices[j].y = polygons[i].vertices[j].y + entityY;
      }
    }

    for (size_t i = 0; i < capsules.size(); ++i) {
      worldCapsules[i].start.x = capsules[i].start.x + entityX;
      worldCapsules[i].start.y = capsules[i].start.y + entityY;
      worldCapsules[i].end.x = capsules[i].end.x + entityX;
      worldCapsules[i].end.y = capsules[i].end.y + entityY;
      worldCapsules[i].r = capsules[i].r;
    }
  }
}
