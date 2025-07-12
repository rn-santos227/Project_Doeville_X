#ifndef PHYSICS_COMPONENT_H
#define PHYSICS_COMPONENT_H

#include "components/BaseComponent.h"
#include "libraries/constants/Constants.h"

namespace Project { namespace Entities { class Entity; } }

namespace Project::Components {
  class PhysicsComponent : public BaseComponent {
  public:

  private:
    Project::Entities::Entity* owner = nullptr;
    
    float velocityX = 0.0f;
    float velocityY = 0.0f;
    float accelerationX = 0.0f;
    float accelerationY = 0.0f;
    float friction = Project::Libraries::Constants::DEFAULT_FRICTION;
    float restitution = Project::Libraries::Constants::DEFAULT_BOUNCE_FACTOR;
  };
}

#endif
