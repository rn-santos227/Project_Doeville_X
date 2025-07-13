#ifndef LAYER_CATEGORY_RESOLVER_H
#define LAYER_CATEGORY_RESOLVER_H

#include "LayerCategory.h"
#include <string>

namespace Project::Layers {
  class LayerCategoryResolver {
  public:
    static LayerCategory resolve(const std::string& name);
  };
}

#endif
