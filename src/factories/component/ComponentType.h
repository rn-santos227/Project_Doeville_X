#ifndef COMPONENT_TYPE_H
#define COMPONENT_TYPE_H

namespace Project::Factories {
  enum class ComponentType {
    BOUNDING_BOX,
    BUTTON,
    CURSOR,
    GRAPHICS,
    HUD,
    INPUT,
    PHYSICS,
    TEXT,
    TRANSFORM,
    UNKNOWN
  };
}
#endif
