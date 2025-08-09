#ifndef ASSETS_MANAGER_H
#define ASSETS_MANAGER_H

#include "BaseAsset.h"

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "assets/tile_asset/TileAsset.h"
#include "helpers/objects_manager/ObjectsManager.h"
#include "utilities/logs_manager/LogsManager.h"

namespace Project::Assets {
  class AssetsManager : public Project::Helpers::ObjectsManager<BaseAsset, std::unique_ptr<BaseAsset>> {
  public:
    explicit AssetsManager(Project::Utilities::LogsManager& logsManager);
    ~AssetsManager() = default;

    void addAsset(const std::string& id, std::unique_ptr<BaseAsset> asset);
    bool hasAsset(const std::string& id) const;
    BaseAsset* getAsset(const std::string& id);

    void addTile(const std::string& id, std::unique_ptr<TileAsset> tile);
    TileAsset* getTile(const std::string& id);
    
    std::vector<BaseAsset*> getAssetsByTag(const std::string& tag) const;
    std::vector<BaseAsset*> getAssetsByCategory(AssetCategory category) const;

  private:
    Project::Utilities::LogsManager& logsManager;
    
    std::unordered_map<std::string, std::vector<std::string>> tagMap;
    std::unordered_map<AssetCategory, std::vector<std::string>> categoryMap;
    std::unordered_map<char, TileAsset> tiles;
  };

}

#endif
