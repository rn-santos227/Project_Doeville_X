#include "MapAsset.h"

#include <fstream>
#include <sstream>

#include "libraries/keys/LuaAssetKeys.h"
#include "utilities/lua_state_wrapper/LuaStateWrapper.h"

namespace Project::Assets {
  using Project::Utilities::LogsManager;
  using Project::Utilities::LuaStateWrapper;
  using Project::Handlers::ResourcesHandler;

  namespace Keys = Project::Libraries::Keys;
  MapAsset::MapAsset(SDL_Renderer* renderer, LogsManager& logsManager, ResourcesHandler& resourcesHandler)
    : BaseAsset(renderer, logsManager, resourcesHandler) {}
}
