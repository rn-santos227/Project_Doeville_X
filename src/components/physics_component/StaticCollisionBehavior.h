#ifndef STATIC_COLLISION_BEHAVIOR
#define STATIC_COLLISION_BEHAVIOR

namespace Project::Components::Physics {
  enum class StaticCollisionBehavior {
    BOUNCE,
    DESTROY_ON_HIT,
    GHOST_PASS,
    REST,
    SLIDE,
    STICK,
    TRIGGER_EVENT
  };
}

#endif
