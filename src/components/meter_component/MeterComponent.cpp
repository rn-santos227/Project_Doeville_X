#include "MeterComponent.h"

#include <sstream>
#include <algorithm>
#include <cmath>

#include "components/numeric_component/NumericComponent.h"
#include "entities/Entity.h"
#include "entities/EntitiesManager.h"
#include "libraries/categories/ComponentCategories.h"
#include "libraries/constants/Constants.h"
#include "libraries/keys/Keys.h"
#include "services/styling/StyleManager.h"
#include "utilities/color/ColorUtils.h"
#include "utilities/math/MathUtils.h"
#include "utilities/geometry/GeometryUtils.h"

namespace Project::Components {
  using Project::Utilities::ColorUtils;
  using Project::Utilities::MathUtils;
  using Project::Services::Style;
  using Project::Services::StyleManager;

  namespace Constants = Project::Libraries::Constants;
  namespace Keys = Project::Libraries::Keys;

  MeterComponent::MeterComponent(SDL_Renderer* renderer, Project::Utilities::LogsManager& logsManager)
    : BaseComponent(logsManager), renderer(renderer) {}

  void MeterComponent::update(float /*deltaTime*/) {
    if (!numericComponent || numericName.empty()) return;
    float value = numericComponent->getValue(numericName);
    float limit = numericComponent->getLimit(numericName);
    if (limit <= 0.0f) return;
    float ratio = value / limit;
    if (data.isRound) {
      int target = static_cast<int>(ratio * Constants::ANGLE_360_DEG);
      data.currentAngle = MathUtils::interpolate(data.currentAngle, target, data.interpolationSpeed);
    } else {
      int target = (data.orientation == MeterOrientation::HORIZONTAL) ? static_cast<int>(ratio * data.rect.w) : static_cast<int>(ratio * data.rect.h);
      data.currentValue = MathUtils::interpolate(data.currentValue, target, data.interpolationSpeed);
    }
  }

  void MeterComponent::render() {
    if (!renderer) return;
    if (data.isRound) {
      int cx = data.rect.x + data.rect.w / Constants::INDEX_TWO;
      int cy = data.rect.y + data.rect.h / Constants::INDEX_TWO;
      int r = std::min(data.rect.w, data.rect.h) / Constants::INDEX_TWO;

      SDL_SetRenderDrawColor(renderer, data.backgroundColor.r, data.backgroundColor.g, data.backgroundColor.b, data.backgroundColor.a);
      Project::Utilities::GeometryUtils::renderFilledCircle(renderer, cx, cy, r);
      SDL_SetRenderDrawColor(renderer, data.barColor.r, data.barColor.g, data.barColor.b, data.barColor.a);

      for (int angle = 0; angle < data.currentAngle; ++angle) {
        float rad = static_cast<float>(angle) * Constants::PI / Constants::ANGLE_180_DEG;
        int x = cx + static_cast<int>(std::cos(rad) * r);
        int y = cy - static_cast<int>(std::sin(rad) * r);
        SDL_RenderDrawLine(renderer, cx, cy, x, y);
      }
      if (data.borderWidth > 0) {
        SDL_SetRenderDrawColor(renderer, data.borderColor.r, data.borderColor.g, data.borderColor.b, data.borderColor.a);
        for (int i = 0; i < data.borderWidth; ++i) {
          Project::Utilities::GeometryUtils::renderCircle(renderer, cx, cy, r - i);
        }
      }
      return;
    }

    SDL_SetRenderDrawColor(renderer, data.backgroundColor.r, data.backgroundColor.g, data.backgroundColor.b, data.backgroundColor.a);
    if (data.cornerRadius > 0) {
      Project::Utilities::GeometryUtils::renderFilledRoundedRect(renderer, data.rect, data.cornerRadius);
    } else {
      SDL_RenderFillRect(renderer, &data.rect);
    }

    SDL_Rect barRect;
    if (data.orientation == MeterOrientation::HORIZONTAL) {
      barRect = {data.rect.x, data.rect.y, data.currentValue, data.rect.h};
    } else {
      barRect = {data.rect.x, data.rect.y + (data.rect.h - data.currentValue), data.rect.w, data.currentValue};
    }

    SDL_SetRenderDrawColor(renderer, data.barColor.r, data.barColor.g, data.barColor.b, data.barColor.a);
    if (data.cornerRadius > 0) {
      Project::Utilities::GeometryUtils::renderFilledRoundedRect(renderer, barRect, data.cornerRadius);
    } else {
      SDL_RenderFillRect(renderer, &barRect);
    }

    if (data.borderWidth > 0) {
      SDL_SetRenderDrawColor(renderer, data.borderColor.r, data.borderColor.g, data.borderColor.b, data.borderColor.a);
      for (int i = 0; i < data.borderWidth; ++i) {
        SDL_Rect bRect{data.rect.x + i, data.rect.y + i, data.rect.w - 2 * i, data.rect.h - 2 * i};
        int radius = std::max(0, data.cornerRadius - i);
        if (radius > 0) {
          Project::Utilities::GeometryUtils::renderRoundedRect(renderer, bRect, radius);
        } else {
          SDL_RenderDrawRect(renderer, &bRect);
        }
      }
    }
  }
}
