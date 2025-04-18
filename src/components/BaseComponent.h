#ifndef BASE_COMPONENT_H
#define BASE_COMPONENT_H

namespace Project::Components {
  class BaseComponent {
  public:
    virtual ~BaseComponent() = default;
    
    virtual void initialize() {}

    virtual void update(float deltaTime) = 0;
    virtual void render() = 0;

    virtual void shutdown() {}
  };
}

#endif