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
      SDL_FRect rectF{
        static_cast<float>(data.rect.x), 
        static_cast<float>(data.rect.y),
        static_cast<float>(data.rect.w),
        static_cast<float>(data.rect.h)
      };
      
      Project::Utilities::GeometryUtils::renderFilledRoundedRect(renderer, rectF, data.cornerRadius);
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
      SDL_FRect barF{
        static_cast<float>(barRect.x),
        static_cast<float>(barRect.y),
        static_cast<float>(barRect.w),
        static_cast<float>(barRect.h)
      };
      
      Project::Utilities::GeometryUtils::renderFilledRoundedRect(renderer, barF, data.cornerRadius);
    } else {
      SDL_RenderFillRect(renderer, &barRect);
    }

    if (data.borderWidth > 0) {
      SDL_SetRenderDrawColor(renderer, data.borderColor.r, data.borderColor.g, data.borderColor.b, data.borderColor.a);
      for (int i = 0; i < data.borderWidth; ++i) {
        SDL_Rect bRect{data.rect.x + i, data.rect.y + i, data.rect.w - Constants::INDEX_TWO * i, data.rect.h - Constants::INDEX_TWO * i};
        int radius = std::max(0, data.cornerRadius - i);
        if (radius > 0) {
          SDL_FRect bRectF{
            static_cast<float>(bRect.x), 
            static_cast<float>(bRect.y),
            static_cast<float>(bRect.w),
            static_cast<float>(bRect.h)
          };
          
          Project::Utilities::GeometryUtils::renderRoundedRect(renderer, bRectF, radius);
        } else {
          SDL_RenderDrawRect(renderer, &bRect);
        }
      }
    }
  }

  void MeterComponent::build(Project::Utilities::LuaStateWrapper& luaStateWrapper, const std::string& tableName) {
    data.offsetX = static_cast<int>(luaStateWrapper.getTableNumber(tableName, Keys::X, 0.0f));
    data.offsetY = static_cast<int>(luaStateWrapper.getTableNumber(tableName, Keys::Y, 0.0f));
    int width = static_cast<int>(luaStateWrapper.getTableNumber(tableName, Keys::WIDTH, Constants::DEFAULT_COMPONENT_SIZE));
    int height = static_cast<int>(luaStateWrapper.getTableNumber(tableName, Keys::HEIGHT, Constants::DEFAULT_COMPONENT_SIZE));
    setSize(width, height);

    std::string colorHex = luaStateWrapper.getTableString(tableName, Keys::COLOR_HEX, Constants::COLOR_HEX_GREEN);
    Uint8 alpha = static_cast<Uint8>(luaStateWrapper.getTableNumber(tableName, Keys::COLOR_ALPHA, Constants::FULL_ALPHA));
    data.barColor = ColorUtils::hexToRGB(colorHex, alpha);
    data.cornerRadius = static_cast<int>(luaStateWrapper.getTableNumber(tableName, Keys::BORDER_RADIUS, 0));

    std::string orientation = luaStateWrapper.getTableString(tableName, Keys::ORIENTATION, Constants::STR_VERTICAL);
    if (orientation == Constants::STR_HORIZONTAL) data.orientation = MeterOrientation::HORIZONTAL; else data.orientation = MeterOrientation::VERTICAL;
    data.isRound = luaStateWrapper.getTableBoolean(tableName, Keys::ROUND, false);

    std::string name = luaStateWrapper.getTableString(tableName, Keys::NAME, Constants::EMPTY_STRING);
    numericName = name;
    targetName = luaStateWrapper.getTableString(tableName, Keys::TARGET, Constants::EMPTY_STRING);
  }

  void MeterComponent::onAttach() {
    if (!owner) return;
    Project::Entities::Entity* target = owner;
    if (targetName.empty()) {
      targetName = owner->getEntityID();
    } else if (auto* mgr = owner->getEntitiesManager()) {
      auto ent = mgr->getEntity(targetName);
      if (ent) {
        target = ent.get();
      } else {
        targetName = owner->getEntityID();
      }
    }

    if (auto* comp = target->getComponent(Project::Libraries::Categories::Components::NUMERIC_COMPONENT)) {
      if (auto* num = dynamic_cast<NumericComponent*>(comp)) {
        setNumericComponent(num, numericName);
      }
    }
  }

  void MeterComponent::applyStyle() {
    std::istringstream classes(getClass());
    std::string cls;
    while (classes >> cls) {
      Style s = StyleManager::getInstance().getStyle("." + cls);
      if (s.width > 0) data.rect.w = s.width;
      if (s.height > 0) data.rect.h = s.height;
      if (s.background.a != 0) data.backgroundColor = s.background;
      if (s.foreground.a != 0) data.barColor = s.foreground;
      if (s.borderColor.a != 0) data.borderColor = s.borderColor;
      if (s.borderWidth > 0) data.borderWidth = s.borderWidth;
      if (s.borderRadius > 0) data.cornerRadius = static_cast<int>(s.borderRadius);
    }
  }

  void MeterComponent::setEntityPosition(float x, float y) {
    data.rect.x = static_cast<int>(x) + data.offsetX;
    data.rect.y = static_cast<int>(y) + data.offsetY;
  }

  void MeterComponent::setSize(int w, int h) {
    data.rect.w = w;
    data.rect.h = h;
  }

  void MeterComponent::setNumericComponent(NumericComponent* comp, const std::string& name) {
    numericComponent = comp;
    numericName = name;
    if (numericComponent) {
      float value = numericComponent->getValue(numericName);
      float limit = numericComponent->getLimit(numericName);
      if (limit > 0.0f) {
        if (data.isRound) {
          data.currentAngle = static_cast<int>((value / limit) * Constants::ANGLE_360_DEG);
        } else if (data.orientation == MeterOrientation::HORIZONTAL) {
          data.currentValue = static_cast<int>((value / limit) * data.rect.w);
        } else {
          data.currentValue = static_cast<int>((value / limit) * data.rect.h);
        }
      }
    }
  }
}
