#ifndef COMPONENT_TYPE_H
#define COMPONENT_TYPE_H

namespace Project::Components {
  enum class ComponentType {
    BEHAVIOR,
    BOUNDING_BOX,
    BUTTON,
    CAMERA,
    CURSOR,
    GRAPHICS,
    HUD,
    INPUT,
    KEYS,
    LIGHT,
    METER,
    MODAL,
    MOTION,
    NETWORK,
    NUMERIC,
    PHYSICS,
    PORTAL,
    SPAWNER,
    TEXT,
    TIMER,
    TRANSFORM,
    VISION,
    UNKNOWN
  };
}

#endif
