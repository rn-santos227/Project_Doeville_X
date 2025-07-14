#ifndef COMPONENT_TYPE_H
#define COMPONENT_TYPE_H

namespace Project::Factories {
  enum class ComponentType {
    CAMERA,
    BOUNDING_BOX,
    BUTTON,
    CURSOR,
    GRAPHICS,
    HUD,
    INPUT,
    KEYS,
    MOTION,
    PHYSICS,
    TEXT,
    TRANSFORM,
    UNKNOWN
  };
}

#endif
