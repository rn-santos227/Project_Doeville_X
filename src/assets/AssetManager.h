#ifndef ASSETS_MANAGER_H
#define ASSETS_MANAGER_H

#include "BaseAsset.h"

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>


#include "helpers/objects_manager/ObjectsManager.h"

namespace Project::Assets {
  class AssetsManager : public Project::Helpers::ObjectsManager<BaseAsset, std::unique_ptr<BaseAsset>> {
  public:
    AssetsManager() = default;
    ~AssetsManager() = default;

  private:
    std::unordered_map<std::string, std::vector<std::string>> tagMap;
    std::unordered_map<AssetCategory, std::vector<std::string>> categoryMap;
  };

}

#endif
