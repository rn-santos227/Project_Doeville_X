#ifndef RENDERABLE_H
#define RENDERABLE_H

namespace Project::Interfaces {
  class Renderable {
  public:
    virtual ~Renderable() = default;
    virtual void render() = 0;
  };
}

#endif
