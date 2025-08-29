#ifndef DIMENSION_MODE_RESOLVER_H
#define DIMENSION_MODE_RESOLVER_H

#include "DimensionMode.h"
#include <string_view>
namespace Project::States {
  class DimensionModeResolver {
  public:
    static DimensionMode resolve(std::string_view name);
  };
}

#endif
