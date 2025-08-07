#ifndef ASSET_DATA_H
#define ASSET_DATA_H

#include <string>

#include "AssetCategory.h"

namespace Project::Assets {
  struct BaseAssetData {
    std::string name;
    std::string path;
    std::string tag;
    AssetCategory category = AssetCategory::TEXTURE;
  };
}

#endif
