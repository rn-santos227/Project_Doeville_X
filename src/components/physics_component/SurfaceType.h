#ifndef SURFACE_TYPE_H
#define SURFACE_TYPE_H

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
