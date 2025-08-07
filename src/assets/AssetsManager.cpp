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

  BaseAsset* AssetsManager::getAsset(const std::string& id) {
    auto it = objects.find(id);
    return it != objects.end() ? it->second.get() : nullptr;
  }

  std::vector<BaseAsset*> AssetsManager::getAssetsByTag(const std::string& tag) const {
    std::vector<BaseAsset*> results;
    auto it = tagMap.find(tag);
    if (it != tagMap.end()) {
      for (const auto& name : it->second) {
        auto objIt = objects.find(name);
        if (objIt != objects.end()) {
          results.push_back(objIt->second.get());
        }
      }
    }
    return results;
  }

  std::vector<BaseAsset*> AssetsManager::getAssetsByCategory(AssetCategory category) const {
    std::vector<BaseAsset*> results;
    auto it = categoryMap.find(category);
  }
}
