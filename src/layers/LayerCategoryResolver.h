#ifndef LAYER_CATEGORY_RESOLVER_H
#define LAYER_CATEGORY_RESOLVER_H

#include "LayerCategory.h"
#include <string_view>

namespace Project::Layers {
  class LayerCategoryResolver {
  public:
    static LayerCategory resolve(std::string_view name);
  };
}

#endif
