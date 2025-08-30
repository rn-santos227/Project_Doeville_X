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
  VisionComponent::VisionComponent(SDL_Renderer* renderer, LogsManager& logsManager)
    : BaseComponent(logsManager), renderer(renderer) {}

  void VisionComponent::update(float) {
    if (!isActive()) return;
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

  }

  void VisionComponent::setEntityReference(Project::Entities::Entity* entity) {
    owner = entity;
    entitiesManager = entity ? entity->getEntitiesManager() : nullptr;
  }
 
  bool VisionComponent::canSee(const Project::Entities::Entity* target) const {
    return std::find(visibleEntities.begin(), visibleEntities.end(), target) != visibleEntities.end();
  }

  void VisionComponent::renderMask(SDL_Renderer* target) {
    using namespace Project::Libraries::Constants;
    if (!target || !data.revealDarkness) return;
    if (endpoints.size() < INDEX_TWO) return;

    SDL_Color c{0, 0, 0, 0};
    SDL_Vertex verts[INDEX_THREE];
    for (size_t i = 0; i + 1 < endpoints.size(); ++i) {
      verts[INDEX_ZERO] = {{data.position.x, data.position.y}, c, {0, 0}};
      verts[INDEX_ONE] = {{endpoints[i].x, endpoints[i].y}, c, {0, 0}};
      verts[INDEX_TWO] = {{endpoints[i + 1].x, endpoints[i + 1].y}, c, {0, 0}};
      SDL_RenderGeometry(target, nullptr, verts, INDEX_THREE, nullptr, 0);
    }

    if (data.shape == VisionShape::CIRCLE && endpoints.size() > INDEX_TWO) {
      verts[INDEX_ZERO] = {{data.position.x, data.position.y}, c, {0, 0}};
      verts[INDEX_ONE] = {{endpoints.back().x, endpoints.back().y}, c, {0, 0}};
      verts[INDEX_TWO] = {{endpoints.front().x, endpoints.front().y}, c, {0, 0}};
      SDL_RenderGeometry(target, nullptr, verts, INDEX_THREE, nullptr, 0);
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
