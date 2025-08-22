#include "AssetsManager.h"

#include "map_asset/MapAsset.h"
#include "texture_asset/TextureAsset.h"

namespace Project::Assets {
  using Project::Utilities::LogsManager;
  using Project::Handlers::ResourcesHandler;
  using Project::Helpers::ObjectsManager;

  AssetsManager::AssetsManager(LogsManager& logsManager, ResourcesHandler& resourcesHandler)
    : logsManager(logsManager), resourcesHandler(resourcesHandler) {}

  void AssetsManager::addAsset(const std::string& id, std::unique_ptr<BaseAsset> asset) {
    if (!asset) return;
    tagMap[asset->getTag()].push_back(id);
    categoryMap[asset->getCategory()].push_back(id);
    if (asset->getCategory() == AssetCategory::TILE) {
      addTile(id, static_cast<TileAsset*>(asset.get()));
    } else if (asset->getCategory() == AssetCategory::MAP) {
      addMap(id, static_cast<MapAsset*>(asset.get()));
    }
    ObjectsManager<BaseAsset, std::unique_ptr<BaseAsset>>::add(id, std::move(asset));
  }

  bool AssetsManager::hasAsset(const std::string& id) const {
    return objects.find(id) != objects.end();
  }

  BaseAsset* AssetsManager::getAsset(const std::string& id) {
    auto it = objects.find(id);
    return it != objects.end() ? it->second.get() : nullptr;
  }

  void AssetsManager::addTile(const std::string& id, TileAsset* tile) {
    if (id.empty() || !tile) return;
    tiles[id] = tile;
  }

  TileAsset* AssetsManager::getTile(const std::string& id) {
    auto it = tiles.find(id);
    return it != tiles.end() ? it->second : nullptr;
  }

  void AssetsManager::addMap(const std::string& id, MapAsset* map) {
    if (id.empty() || !map) return;
    maps[id] = map;
  }

  MapAsset* AssetsManager::getMap(const std::string& id) {
    auto it = maps.find(id);
    return it != maps.end() ? it->second : nullptr;
  }

  SDL_Texture* AssetsManager::getTexture(const std::string& id) {
    auto* base = getAsset(id);
    if (auto* texture = dynamic_cast<TextureAsset*>(base)) {
      return texture->getTexture();
    }
    return nullptr;
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
    if (it != categoryMap.end()) {
      for (const auto& name : it->second) {
        auto objIt = objects.find(name);
        if (objIt != objects.end()) {
          results.push_back(objIt->second.get());
        }
      }
    }
    return results;
  }
}
