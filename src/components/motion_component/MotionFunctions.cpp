#include "MotionComponent.h"
#include "MotionFunctions.h"

#include <cmath>

#include "components/bounding_box_component/BoundingBoxComponent.h"
#include "components/physics_component/PhysicsComponent.h"
#include "entities/Entity.h"
#include "libraries/constants/Constants.h"
#include "libraries/categories/ComponentCategories.h"
#include "utilities/math/MathUtils.h"

namespace Project::Components {
  using Project::Utilities::MathUtils;

  namespace Components = Project::Libraries::Categories::Components;
  namespace Constants = Project::Libraries::Constants;

  float MotionFunctions::getCurrentSpeed(const MotionComponent* comp) {
    if (!comp) return 0.0f;
    auto* owner = comp->getOwner();
    if (owner) {
      if (auto* physics = dynamic_cast<PhysicsComponent*>(owner->getComponent(Components::PHYSICS_COMPONENT))) {
        float vx = physics->getVelocityX();
        float vy = physics->getVelocityY();
        return MathUtils::magnitude(vx, vy);
      }
    }
    return MathUtils::magnitude(comp->velocityX, comp->velocityY);
  }
}
