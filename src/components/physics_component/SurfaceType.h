#ifndef SURFACE_TYPE
#define SURFACE_TYPE

namespace Project::Components::Physics {
  enum class SurfaceType {
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
