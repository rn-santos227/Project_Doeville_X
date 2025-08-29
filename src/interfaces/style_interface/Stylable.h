#ifndef STYLABLE_H
#define STYLABLE_H

namespace Project::Interfaces {
  class Stylable {
  public:
    virtual ~Stylable() = default;
    virtual void applyStyle() = 0;
  };
}

#endif
