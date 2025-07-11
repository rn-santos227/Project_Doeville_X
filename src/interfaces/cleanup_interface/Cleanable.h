#ifndef CLEANABLE_H
#define CLEANABLE_H

namespace Project::Interfaces {
  class Cleanable {
  public:
    virtual ~Cleanable() = default;
    virtual void cleanup() = 0;
  };
}

#endif 
