#include "AssetsFactory.h"

#include "assets/BaseAsset.h"
#include "assets/AssetCategoryResolver.h"
#include "utilities/lua_state_wrapper/LuaStateWrapper.h"

namespace Project::Factories {
  using Project::Assets::BaseAsset;
  using Project::Assets::AssetsManager;
  using Project::Assets::AssetCategory;
  using Project::Assets::AssetCategoryResolver;
  using Project::Handlers::ResourcesHandler;
  using Project::Utilities::LogsManager;
  using Project::Utilities::LuaStateWrapper;

  AssetsFactory::AssetsFactory(
    SDL_Renderer* renderer,
    LogsManager& logsManager,
    ResourcesHandler& resourcesHandler,
    AssetsManager& assetsManager)
    : renderer(renderer),
      logsManager(logsManager),
      resourcesHandler(resourcesHandler),
      assetsManager(assetsManager) {}
}
