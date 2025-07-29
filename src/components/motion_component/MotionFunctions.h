#ifndef MOTION_FUNCTIONS_H
#define MOTION_FUNCTIONS_H

namespace Project { namespace Components { class MotionComponent; class PhysicsComponent; } }

namespace Project::Components {
  class MotionFunctions {
  public:
    static void brake(MotionComponent* comp);
    static void turn(MotionComponent* comp, float speed, bool left);
  };
}

#endif
