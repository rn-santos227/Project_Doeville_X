#ifndef COMPONENT_TYPE_H
#define COMPONENT_TYPE_H

namespace Project::Factories {
  enum class ComponentType {
    BOUNDING_BOX,
    BUTTON,
    CAMERA,
    CURSOR,
    GRAPHICS,
    HUD,
    INPUT,
    KEYS,
    MOTION,
    PHYSICS,
    TEXT,
    TIMER,
    TRANSFORM,
    UNKNOWN
  };
}

#endif
