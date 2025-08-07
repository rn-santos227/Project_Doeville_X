#include "AssetsManager.h"

namespace Project::Assets {
  using Project::Helpers::ObjectsManager;

  void AssetsManager::addAsset(const std::string& id, std::unique_ptr<BaseAsset> asset) {
    if (!asset) return;
    tagMap[asset->getTag()].push_back(id);
    categoryMap[asset->getCategory()].push_back(id);
    ObjectsManager<BaseAsset, std::unique_ptr<BaseAsset>>::add(id, std::move(asset));
  }

  bool AssetsManager::hasAsset(const std::string& id) const {
    return objects.find(id) != objects.end();
  }
}
