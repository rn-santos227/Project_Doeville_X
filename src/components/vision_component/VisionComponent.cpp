#include "VisionComponent.h"

#include <algorithm>
#include <cmath>
#include <string>
#include <unordered_set>

#include "libraries/constants/Constants.h"
#include "libraries/keys/Keys.h"
#include "utilities/string/StringUtils.h"

namespace Project::Components {
  using Project::Utilities::LogsManager;
  Project::Handlers::CameraHandler* VisionComponent::cameraHandler = nullptr;

  void VisionComponent::setCameraHandler(Project::Handlers::CameraHandler* handler) {
    cameraHandler = handler;
  }
  
  VisionComponent::VisionComponent(SDL_Renderer* renderer, LogsManager& logsManager)
    : BaseComponent(logsManager), renderer(renderer) {}

  void VisionComponent::update(float) {
    if (!isActive()) return;
    if (owner) {
      setEntityPosition(owner->getX(), owner->getY());
    }
    castRays();
  }

  void VisionComponent::render() {
    if (!isActive()) return;
  }

  void VisionComponent::build(Project::Utilities::LuaStateWrapper& luaStateWrapper, const std::string& tableName) {
    namespace Keys = Project::Libraries::Keys;
    namespace Constants = Project::Libraries::Constants;

    std::string shape = luaStateWrapper.getTableString(tableName, Keys::SHAPE, Constants::CONE);
    if (Project::Utilities::StringUtils::iequals(shape, Constants::CIRCLE)) {
      data.shape = VisionShape::CIRCLE;
    } else {
      data.shape = VisionShape::CONE;
    }

    data.radius = luaStateWrapper.getTableNumber(tableName, Keys::RADIUS, Constants::DEFAULT_LIGHT_RADIUS);
    data.angle = luaStateWrapper.getTableNumber(tableName, Keys::ANGLE, Constants::ANGLE_90_DEG);
    data.direction = luaStateWrapper.getTableNumber(tableName, Keys::DIRECTION, Constants::ANGLE_0_DEG);
    data.offsetX = luaStateWrapper.getTableNumber(tableName, Keys::X, 0.0f);
    data.offsetY = luaStateWrapper.getTableNumber(tableName, Keys::Y, 0.0f);
    data.rays = static_cast<int>(luaStateWrapper.getTableNumber(tableName, Keys::RAYS, Constants::DEFAULT_LIGHT_RAY_COUNT));
    data.revealDarkness = luaStateWrapper.getTableBoolean(tableName, Keys::REVEAL_DARKNESS, false);
  }

  void VisionComponent::setEntityPosition(float x, float y) {
    data.position = {
      x + positionOffset.x + data.offsetX,
      y + positionOffset.y + data.offsetY
    };
  }

  void VisionComponent::setEntityReference(Project::Entities::Entity* entity) {
    owner = entity;
    entitiesManager = entity ? entity->getEntitiesManager() : nullptr;
    positionOffset = {0.f, 0.f};
    if (!entity) return;
    if (auto* box = entity->getBoundingBoxComponent()) {
      const auto& boxes = box->getBoxes();
      if (!boxes.empty()) {
        positionOffset.x = boxes[0].w * 0.5f;
        positionOffset.y = boxes[0].h * 0.5f;
      }
    }
  }
 
  bool VisionComponent::canSee(const Project::Entities::Entity* target) const {
    return std::find(visibleEntities.begin(), visibleEntities.end(), target) != visibleEntities.end();
  }

  void VisionComponent::renderMask(SDL_Renderer* target) {
    using namespace Project::Libraries::Constants;
    if (!target || !data.revealDarkness) return;
    if (endpoints.size() < INDEX_TWO) return;

    float camX = 0.0f, camY = 0.0f, zoom = DEFAULT_WHOLE;
    if (cameraHandler) {
      camX = std::floor(cameraHandler->getX());
      camY = std::floor(cameraHandler->getY());
      zoom = cameraHandler->getZoom();
    }

    auto toScreen = [&](const SDL_FPoint& p) {
      return SDL_FPoint{(p.x - camX) * zoom, (p.y - camY) * zoom};
    };

    SDL_FPoint renderPos = toScreen(data.position);

    const float fade = 20.f;
    SDL_Color transparent{0, 0, 0, 0};
    SDL_Color edge{0, 0, 0, FULL_ALPHA};

    auto computeInner = [&](const SDL_FPoint& p) {
      float dx = p.x - data.position.x;
      float dy = p.y - data.position.y;
      float dist = std::hypot(dx, dy);
      if (dist <= fade) return SDL_FPoint{data.position.x, data.position.y};
      float scale = (dist - fade) / dist;
      return SDL_FPoint{data.position.x + dx * scale, data.position.y + dy * scale};
    };

    size_t n = endpoints.size();
    if (data.shape == VisionShape::CIRCLE) {
      for (size_t i = 0; i < n; ++i) {
        size_t j = (i + 1) % n;
        SDL_FPoint p0 = endpoints[i];
        SDL_FPoint p1 = endpoints[j];
        SDL_FPoint i0 = computeInner(p0);
        SDL_FPoint i1 = computeInner(p1);

        SDL_FPoint sp0 = toScreen(p0);
        SDL_FPoint sp1 = toScreen(p1);
        SDL_FPoint si0 = toScreen(i0);
        SDL_FPoint si1 = toScreen(i1);

        SDL_Vertex innerTri[INDEX_THREE] = {
          {renderPos, transparent, {0, 0}}, 
          {si0, transparent, {0, 0}},
          {si1, transparent, {0, 0}}
        };
        SDL_RenderGeometry(target, nullptr, innerTri, INDEX_THREE, nullptr, 0);

        SDL_Vertex ringTri1[INDEX_THREE] = {
          {si0, transparent, {0, 0}},
          {sp0, edge, {0, 0}},
          {sp1, edge, {0, 0}}
        };
        
        SDL_Vertex ringTri2[INDEX_THREE] = {
          {si0, transparent, {0, 0}},
          {sp1, edge, {0, 0}},
          {si1, transparent, {0, 0}}
        };
        
        SDL_RenderGeometry(target, nullptr, ringTri1, INDEX_THREE, nullptr, 0);
        SDL_RenderGeometry(target, nullptr, ringTri2, INDEX_THREE, nullptr, 0);
      }
    } else {
      for (size_t i = 0; i + 1 < n; ++i) {
        SDL_FPoint p0 = endpoints[i];
        SDL_FPoint p1 = endpoints[i + 1];
        SDL_FPoint i0 = computeInner(p0);
        SDL_FPoint i1 = computeInner(p1);

        SDL_FPoint sp0 = toScreen(p0);
        SDL_FPoint sp1 = toScreen(p1);
        SDL_FPoint si0 = toScreen(i0);
        SDL_FPoint si1 = toScreen(i1);

        SDL_Vertex innerTri[INDEX_THREE] = {
          {renderPos, transparent, {0, 0}},
          {si0, transparent, {0, 0}},
          {si1, transparent, {0, 0}}
        };
        SDL_RenderGeometry(target, nullptr, innerTri, INDEX_THREE, nullptr, 0);

        SDL_Vertex ringTri1[INDEX_THREE] = {
          {si0, transparent, {0, 0}},
          {sp0, edge, {0, 0}},
          {sp1, edge, {0, 0}}
        };
        
        SDL_Vertex ringTri2[INDEX_THREE] = {
          {si0, transparent, {0, 0}},
          {sp1, edge, {0, 0}},
          {si1, transparent, {0, 0}}
        };
        
        SDL_RenderGeometry(target, nullptr, ringTri1, INDEX_THREE, nullptr, 0);
        SDL_RenderGeometry(target, nullptr, ringTri2, INDEX_THREE, nullptr, 0);
      }
      
      SDL_FPoint pStart = endpoints.front();
      SDL_FPoint pEnd = endpoints.back();
      SDL_FPoint iStart = computeInner(pStart);
      SDL_FPoint iEnd = computeInner(pEnd);

      SDL_FPoint spStart = toScreen(pStart);
      SDL_FPoint spEnd = toScreen(pEnd);
      SDL_FPoint siStart = toScreen(iStart);
      SDL_FPoint siEnd = toScreen(iEnd);


      SDL_Vertex side1[INDEX_THREE] = {
        {renderPos, transparent, {0, 0}},
        {siStart, transparent, {0, 0}},
        {spStart, edge, {0, 0}}
      };
      
      SDL_Vertex side2[INDEX_THREE] = {
        {renderPos, transparent, {0, 0}},
        {spEnd, edge, {0, 0}},
        {siEnd, transparent, {0, 0}}
      };
      
      SDL_RenderGeometry(target, nullptr, side1, INDEX_THREE, nullptr, 0);
      SDL_RenderGeometry(target, nullptr, side2, INDEX_THREE, nullptr, 0);
    }
  }

  std::pair<SDL_FPoint, Project::Entities::Entity*> VisionComponent::castSingleRay(float angleDegrees) {
    using namespace Project::Libraries::Constants;
    float rad = angleDegrees * DEG_TO_RAD;
    float dx = std::cos(rad);
    float dy = std::sin(rad);

    SDL_FPoint end{data.position.x + dx * data.radius, data.position.y + dy * data.radius};
    float closest = data.radius;
    Project::Entities::Entity* hitEntity = nullptr;

    if (!entitiesManager) return {end, nullptr};

    float origin[2] = {data.position.x, data.position.y};
    float dir[2] = {dx, dy};

    for (auto* base : entitiesManager->getComponentArray(ComponentType::BOUNDING_BOX)) {
      auto* box = dynamic_cast<BoundingBoxComponent*>(base);
      if (!box || !box->isActive() || !box->isSolid()) continue;
      for (const auto& rect : box->getBoxes()) {
        float t;
        if (rayIntersectsAABB(origin, dir, rect, t) &&
            t > ANGLE_0_DEG && t < closest) {
          closest = t;
          end = {origin[0] + dir[0] * t, origin[1] + dir[1] * t};
          hitEntity = box->getOwner();
        }
      }
    }

    for (auto* base : entitiesManager->getComponentArray(ComponentType::GRAPHICS)) {
      auto* gfx = dynamic_cast<GraphicsComponent*>(base);
      if (!gfx || !gfx->isActive() || !gfx->isOccluder() || gfx->getOwner() == owner) continue;
      float t;
      if (rayIntersectsAABB(origin, dir, gfx->getBoundingBox(), t) &&
          t > ANGLE_0_DEG && t < closest) {
        closest = t;
        end = {origin[0] + dir[0] * t, origin[1] + dir[1] * t};
        hitEntity = gfx->getOwner();
      }
    }

    return {end, hitEntity};
  }

  bool VisionComponent::rayIntersectsAABB(const float origin[2], const float dir[2], const SDL_FRect& rect, float& outT) const {
    using namespace Project::Libraries::Constants;
    float tmin = ANGLE_0_DEG;
    float tmax = RAYCAST_MAX_DISTANCE;

    float minB[2] = {rect.x, rect.y};
    float maxB[2] = {rect.x + rect.w, rect.y + rect.h};

    for (int i = 0; i < INDEX_TWO; ++i) {
      if (std::abs(dir[i]) < RAYCAST_EPSILON) {
        if (origin[i] < minB[i] || origin[i] > maxB[i]) return false;
      } else {
        float ood = DEFAULT_WHOLE / dir[i];
        float t1 = (minB[i] - origin[i]) * ood;
        float t2 = (maxB[i] - origin[i]) * ood;
        if (t1 > t2) std::swap(t1, t2);
        if (t1 > tmin) tmin = t1;
        if (t2 < tmax) tmax = t2;
        if (tmin > tmax) return false;
      }
    }

    outT = tmin;
    return true;
  }

  void VisionComponent::castRays() {
    using namespace Project::Libraries::Constants;
    endpoints.clear();
    visibleEntities.clear();
    float startAngle = ANGLE_0_DEG;
    float endAngle = ANGLE_360_DEG;
    if (data.shape == VisionShape::CONE) {
      startAngle = data.direction - data.angle * DEFAULT_HALF;
      endAngle = data.direction + data.angle * DEFAULT_HALF;
    }
    float step = (endAngle - startAngle) / static_cast<float>(data.rays);
    std::unordered_set<Project::Entities::Entity*> unique;
    for (float a = startAngle; a <= endAngle; a += step) {
      auto [pt, hit] = castSingleRay(a);
      endpoints.emplace_back(pt);
      if (hit && hit != owner) unique.insert(hit);
    }
    visibleEntities.assign(unique.begin(), unique.end());
  }
}
