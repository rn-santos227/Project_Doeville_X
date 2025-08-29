#ifndef POSITIONABLE_COMPONENT_H
#define POSITIONABLE_COMPONENT_H

namespace Project::Components {
  class PositionableComponent {
  public:
    virtual ~PositionableComponent() = default;
    virtual void setEntityPosition(float x, float y) = 0;
  };
}

#endif
