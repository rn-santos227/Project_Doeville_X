#include "AssetsManager.h"

namespace Project::Assets {
  void AssetsManager::addAsset(const std::string& id, std::unique_ptr<BaseAsset> asset) {
    if (!asset) return;
    tagMap[asset->getTag()].push_back(id);
  }
}
