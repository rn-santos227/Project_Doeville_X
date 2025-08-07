#ifndef ASSET_CATEGORY_RESOLVER_H
#define ASSET_CATEGORY_RESOLVER_H

#include "AssetCategory.h"

#include <string_view>

namespace Project::Handlers {
  class AssetCategoryResolver {
  public:
    static AssetCategory resolve(std::string_view name);
  };
}

#endif
