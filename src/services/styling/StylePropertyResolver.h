#ifndef STYLE_PROPERTY_RESOLVER
#define STYLE_PROPERTY_RESOLVER

#include "StyleProperty.h"

#include <string>
#include <unordered_map>

namespace Project::Services {
  class StylePropertyResolver {
  public:
    static StyleProperty resolve(const std::string& property);
  };
}

#endif
