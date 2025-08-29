#include "LightComponent.h"

#include <algorithm>
#include <cmath>

#include "libraries/constants/Constants.h"
#include "libraries/keys/Keys.h"
#include "utilities/color/ColorUtils.h"

namespace Project::Components {
  namespace Constants = Project::Libraries::Constants;

  LightComponent::LightComponent(SDL_Renderer* renderer, Project::Utilities::LogsManager& logsManager)
    : BaseComponent(logsManager), renderer(renderer) {}

  void LightComponent::update(float) {
    if (!isActive()) return;
    castRays();
  }

  void LightComponent::render() {
    if (!isActive() || !renderer) return;
    if (endpoints.size() < Constants::INDEX_TWO) return;

    int w = 0, h = 0;
    SDL_GetRendererOutputSize(renderer, &w, &h);
    SDL_Texture* tex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, w, h);
    if (!tex) return;
    SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_ADD);
    SDL_SetRenderTarget(renderer, tex);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);

    SDL_Color white{Constants::FULL_ALPHA, Constants::FULL_ALPHA, Constants::FULL_ALPHA, Constants::FULL_ALPHA};
    SDL_Vertex verts[Constants::INDEX_THREE];
    for (size_t i = 0; i + 1 < endpoints.size(); ++i) {
      verts[Constants::INDEX_ZERO] = {{data.position.x, data.position.y}, white, {0, 0}};
      verts[Constants::INDEX_ONE] = {{endpoints[i].x, endpoints[i].y}, white, {0, 0}};
      verts[Constants::INDEX_TWO] = {{endpoints[i + 1].x, endpoints[i + 1].y}, white, {0, 0}};
      SDL_RenderGeometry(renderer, nullptr, verts, Constants::INDEX_THREE, nullptr, 0);
    }

    if (data.shape == LightShape::CIRCLE && endpoints.size() > Constants::INDEX_TWO) {
      verts[Constants::INDEX_ZERO] = {{data.position.x, data.position.y}, white, {0, 0}};
      verts[Constants::INDEX_ONE] = {{endpoints.back().x, endpoints.back().y}, white, {0, 0}};
      verts[2] = {{endpoints.front().x, endpoints.front().y}, white, {0, 0}};
      SDL_RenderGeometry(renderer, nullptr, verts, Constants::INDEX_THREE, nullptr, 0);
    }

    SDL_SetRenderTarget(renderer, nullptr);
    Project::Utilities::ColorUtils::applyShader(tex, data.color);
    SDL_RenderCopy(renderer, tex, nullptr, nullptr);
    SDL_DestroyTexture(tex);
  }

  void LightComponent::build(Project::Utilities::LuaStateWrapper& luaStateWrapper, const std::string& tableName) {
    namespace Keys = Project::Libraries::Keys;
    std::string colorHex = luaStateWrapper.getTableString(tableName, Keys::COLOR_HEX, Constants::DEFAULT_LIGHT_COLOR_HEX);
    Uint8 alpha = static_cast<Uint8>(luaStateWrapper.getTableNumber(tableName, Keys::COLOR_ALPHA, Constants::FULL_ALPHA));
    data.color = Project::Utilities::ColorUtils::hexToRGB(colorHex, alpha);
  }

  void LightComponent::setEntityReference(Project::Entities::Entity* entity) {
    owner = entity;
    entitiesManager = entity ? entity->getEntitiesManager() : nullptr;
  }

  void LightComponent::castRays() {
    endpoints.clear();
    float startAngle = Constants::ANGLE_0_DEG;
    float endAngle = Constants::ANGLE_360_DEG;
    if (data.shape == LightShape::CONE) {
      startAngle = data.direction - data.angle * Constants::DEFAULT_HALF;
      endAngle = data.direction + data.angle * Constants::DEFAULT_HALF;
    }
    float step = (endAngle - startAngle) / static_cast<float>(data.rays);
    for (float a = startAngle; a <= endAngle; a += step) {
      endpoints.emplace_back(castSingleRay(a));
    }
  }

  SDL_FPoint LightComponent::castSingleRay(float angleDegrees) {
    float rad = angleDegrees * Constants::DEG_TO_RAD;
    float dx = std::cos(rad);
    float dy = std::sin(rad);

    SDL_FPoint end{data.position.x + dx * data.radius, data.position.y + dy * data.radius};
    float closest = data.radius;

    if (entitiesManager) {
      auto& comps = entitiesManager->getComponentArray(ComponentType::BOUNDING_BOX);
      for (auto* base : comps) {
        auto* box = dynamic_cast<BoundingBoxComponent*>(base);
        if (!box || !box->isActive() || !box->isSolid()) continue;
        for (const auto& rect : box->getBoxes()) {
          float tmin = Constants::ANGLE_0_DEG, tmax = Constants::RAYCAST_MAX_DISTANCE;
          float p[Constants::INDEX_TWO] = {dx, dy};
          float o[Constants::INDEX_TWO] = {data.position.x, data.position.y};
          float minB[Constants::INDEX_TWO] = {rect.x, rect.y};
          float maxB[Constants::INDEX_TWO] = {rect.x + rect.w, rect.y + rect.h};
          bool hit = true;
          for (int i = 0; i < Constants::INDEX_TWO; ++i) {
            if (std::abs(p[i]) < Constants::RAYCAST_EPSILON) {
              if (o[i] < minB[i] || o[i] > maxB[i]) { hit = false; break; }
            } else {
              float ood = Constants::DEFAULT_WHOLE / p[i];
              float t1 = (minB[i] - o[i]) * ood;
              float t2 = (maxB[i] - o[i]) * ood;
              if (t1 > t2) std::swap(t1, t2);
              if (t1 > tmin) tmin = t1;
              if (t2 < tmax) tmax = t2;
              if (tmin > tmax) { hit = false; break; }
            }
          }
          if (hit && tmin > Constants::ANGLE_0_DEG && tmin < closest) {
            closest = tmin;
            end = {o[0] + p[0] * tmin, o[1] + p[1] * tmin};
          }
        }
      }
    }
    return end;
  }
}