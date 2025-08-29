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

    float amount = std::abs(speed);
    if (auto* physics = dynamic_cast<PhysicsComponent*>(owner->getComponent(Components::PHYSICS_COMPONENT))) {
      physics->setRotationEnabled(true);
      physics->setAngularVelocity(left ? -amount : amount);
    } else {
      box->setRotation(box->getRotation() + (left ? -amount : amount));
    }
  }

  void MotionFunctions::handleStandard(MotionComponent* comp, float& velX, float& velY, bool left, bool right, bool up, bool down, float deltaTime, bool hasInput) {
    if (!comp) return;
    if (comp->accelerationEnabled) {
      if (left) {
        velX -= comp->acceleration * deltaTime;
      } else if (right) {
        velX += comp->acceleration * deltaTime;
      } else {
        if (velX > 0.0f) {
          velX -= comp->friction * deltaTime;
          if (velX < 0.0f) velX = 0.0f;
        } else if (velX < 0.0f) {
          velX += comp->friction * deltaTime;
          if (velX > 0.0f) velX = 0.0f;
        }
      }

      if (up) {
        velY -= comp->acceleration * deltaTime;
      } else if (down) {
        velY += comp->acceleration * deltaTime;
      } else {
        if (velY > 0.0f) {
          velY -= comp->friction * deltaTime;
          if (velY < 0.0f) velY = 0.0f;
        } else if (velY < 0.0f) {
          velY += comp->friction * deltaTime;
          if (velY > 0.0f) velY = 0.0f;
        }
      }
    } else if (hasInput) {
      velX = 0.0f;
      velY = 0.0f;

      if (left) {
        velX = -comp->maxSpeed;
      } else if (right) {
        velX = comp->maxSpeed;
      }

      if (up) {
        velY = -comp->maxSpeed;
      } else if (down) {
        velY = comp->maxSpeed;
      }
    }
  }

  void MotionFunctions::handleVehicle(MotionComponent* comp, PhysicsComponent* physics, float& velX, float& velY, bool left, bool right, bool up, bool down, float deltaTime) {
    if (!comp) return;
    if (left) {
      float rot = physics ? physics->getRotationSpeed() : Project::Libraries::Constants::DEFAULT_ROTATION_SPEED;
      MotionFunctions::turn(comp, rot, true);
    } else if (right) {
      float rot = physics ? physics->getRotationSpeed() : Project::Libraries::Constants::DEFAULT_ROTATION_SPEED;
      MotionFunctions::turn(comp, rot, false);
    } else if (physics) {
      physics->setAngularVelocity(0.0f);
    }

    float angle = 0.0f;
    if (physics) {
      angle = physics->getRotation() + physics->getAngularVelocity() * deltaTime;
    } else if (auto* box = dynamic_cast<BoundingBoxComponent*>(comp->getOwner()->getComponent(Project::Libraries::Categories::Components::BOUNDING_BOX_COMPONENT))) {
      angle = box->getRotation();
    }

    float rad = angle * Constants::DEG_TO_RAD;
    float dirX = std::cos(rad);
    float dirY = std::sin(rad);

    if (comp->accelerationEnabled) {
      if (up) {
        velX += dirX * comp->acceleration * deltaTime;
        velY += dirY * comp->acceleration * deltaTime;
      } else if (down) {
        velX -= dirX * comp->acceleration * deltaTime;
        velY -= dirY * comp->acceleration * deltaTime;
      } else {
        float speed = MathUtils::magnitude(velX, velY);
        if (speed > 0.0f) {
          float decel = comp->friction * deltaTime;
          if (decel > speed) decel = speed;
          float scale = (speed - decel) / speed;
          velX *= scale;
          velY *= scale;
        }
      }
    } else {
      velX = 0.0f;
      velY = 0.0f;

      if (up) {
        velX = dirX * comp->maxSpeed;
        velY = dirY * comp->maxSpeed;
      } else if (down) {
        velX = -dirX * comp->maxSpeed;
        velY = -dirY * comp->maxSpeed;
      }
    }

    if ((left || right) && (velX != 0.0f || velY != 0.0f)) {
      float speed = MathUtils::magnitude(velX, velY);
      velX = dirX * speed;
      velY = dirY * speed;
    }
  }
}
