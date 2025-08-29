#ifndef ROTATABLE_H
#define ROTATABLE_H

namespace Project::Interfaces {
  class Rotatable {
  public:
    virtual ~Rotatable() = default;
    virtual void setEntityRotation(float rotation) = 0;
  };
}

#endif
