#ifndef STYLE_PROPERTY_RESOLVER_H
#define STYLE_PROPERTY_RESOLVER_H

#include "StyleProperty.h"
#include <string_view>

namespace Project::Services {
  class StylePropertyResolver {
  public:
    static StyleProperty resolve(std::string_view property);
  };
}

#endif
