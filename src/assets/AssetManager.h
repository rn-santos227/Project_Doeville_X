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
    AssetsManager() = default;
    ~AssetsManager() = default;
  };

}

#endif
