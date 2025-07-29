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

  void MotionFunctions::brake(MotionComponent* comp) {
    if (!comp) return;
    auto* owner = comp->getOwner();

    if (!owner) {
      if (comp->brakePower >= Constants::DEFAULT_WHOLE) {
        comp->velocityX = 0.0f;
        comp->velocityY = 0.0f;
      } else {
        comp->velocityX *= (Constants::DEFAULT_WHOLE - comp->brakePower);
        comp->velocityY *= (Constants::DEFAULT_WHOLE - comp->brakePower);
      }
      return;
    }

    if (auto* physics = dynamic_cast<PhysicsComponent*>(owner->getComponent(Components::PHYSICS_COMPONENT))) {
      if (comp->brakePower >= Constants::DEFAULT_WHOLE) {
        physics->setVelocity(0.0f, 0.0f);
      } else {
        float newVX = physics->getVelocityX() * (Constants::DEFAULT_WHOLE - comp->brakePower);
        float newVY = physics->getVelocityY() * (Constants::DEFAULT_WHOLE - comp->brakePower);
        physics->setVelocity(newVX, newVY);
      }
    } else {
      if (comp->brakePower >= Constants::DEFAULT_WHOLE) {
        comp->velocityX = 0.0f;
        comp->velocityY = 0.0f;
      } else {
        comp->velocityX *= (Constants::DEFAULT_WHOLE - comp->brakePower);
        comp->velocityY *= (Constants::DEFAULT_WHOLE - comp->brakePower);
      }
    }
  }

  void MotionFunctions::turn(MotionComponent* comp, float speed, bool left) {
  if (!comp || !comp->getOwner()) return;
  auto* owner = comp->getOwner();
  auto* box = dynamic_cast<BoundingBoxComponent*>(owner->getComponent(Components::BOUNDING_BOX_COMPONENT));
  if (!box || !box->isRotationEnabled()) return;
  }
}
