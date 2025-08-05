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
}
