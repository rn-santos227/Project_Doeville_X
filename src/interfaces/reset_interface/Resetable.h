#ifndef RESETABLE_H
#define RESETABLE_H

namespace Project::Interfaces {
  class Resetable {
  public:
    virtual ~Resetable() = default;
    virtual void reset() = 0;
  };
}

#endif
