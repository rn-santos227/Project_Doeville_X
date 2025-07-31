#ifndef DIMENSION_MODE_RESOLVER_H
#define DIMENSION_MODE_RESOLVER_H
#include "DimensionMode.h"
#include <string>

namespace Project::States {
  class DimensionModeResolver {
  public:
    static DimensionMode resolve(const std::string& name);
  };
}

#endif
