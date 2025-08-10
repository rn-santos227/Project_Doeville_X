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

  bool MapAsset::loadFromLua(const std::string& scriptPath, const std::string& assetName) {
    LuaStateWrapper lua(logsManager);
    if (!lua.loadScript(scriptPath)) {
      logsManager.logError("Failed to load asset script: " + scriptPath);
      return false;
    }

    lua_State* L = lua.get();
    lua_getglobal(L, Keys::LUA_ASSET_GET_ASSET);
    if (!lua_isfunction(L, -1)) {
      logsManager.logError("Asset script missing getAsset function: " + scriptPath);
      return false;
    }
  }
}
