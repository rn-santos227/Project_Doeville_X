#ifndef BASE_ASSET_H
#define BASE_ASSET_H

#include "BaseAssetData.h"

#include <string>
#include <SDL.h>

#include "handlers/resources/ResourcesHandler.h"
#include "libraries/constants/NameConstants.h"
#include "utilities/logs_manager/LogsManager.h"

namespace Project::Assets {
  class BaseAsset {
  public:
    explicit BaseAsset(
      SDL_Renderer* renderer,
      Project::Utilities::LogsManager& logsManager,
      Project::Handlers::ResourcesHandler& resourcesHandler)
      : renderer(renderer), logsManager(logsManager), resourcesHandler(resourcesHandler) {}
    
    virtual ~BaseAsset() = default;
    BaseAsset(const BaseAsset&) = delete;
    BaseAsset& operator=(const BaseAsset&) = delete;
    BaseAsset(BaseAsset&&) = default;
    BaseAsset& operator=(BaseAsset&&) = default;

    AssetCategory getCategory() const { return data.category; }
    const std::string& getName() const { return data.name; }
    const std::string& getTag() const { return data.tag; }
    
    virtual bool loadFromLua(
      const std::string& scriptPath, 
      const std::string& assetName = Project::Libraries::Constants::EMPTY_STRING
    ) = 0;

  protected:
    SDL_Renderer* renderer;
    Project::Utilities::LogsManager& logsManager;
    Project::Handlers::ResourcesHandler& resourcesHandler;
    BaseAssetData data;
  };
}

#endif
