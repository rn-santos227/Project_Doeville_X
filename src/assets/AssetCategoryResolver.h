#ifndef ASSET_CATEGORY_RESOLVER_H
#define ASSET_CATEGORY_RESOLVER_H

#include "AssetCategory.h"

#include <string_view>

namespace Project::Assets {
  class AssetCategoryResolver {
  public:
    static AssetCategory resolve(std::string_view name);
  };
}

#endif
