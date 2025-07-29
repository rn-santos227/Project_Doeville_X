#ifndef MOTION_FUNCTIONS_H
#define MOTION_FUNCTIONS_H

namespace Project { namespace Components { class MotionComponent; class PhysicsComponent; } }

namespace Project::Components {
  class MotionFunctions {
  public:
    static float getCurrentSpeed(const MotionComponent* comp);
    static void brake(MotionComponent* comp);
    static void turn(MotionComponent* comp, float speed, bool left);

    static void handleStandard(
      MotionComponent* comp,
      float& velX, float& velY,
      bool left, bool right, 
      bool up, bool down,
      float deltaTime, bool hasInput
    );

    static void handleVehicle(
      MotionComponent* comp,
      PhysicsComponent* physics,
      float& velX, float& velY,
      bool left, bool right, 
      bool up, bool down,
      float deltaTime
    );
  };
}

#endif
