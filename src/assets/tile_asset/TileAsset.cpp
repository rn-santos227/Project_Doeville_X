#include "TileAsset.h"

#include "libraries/keys/LuaAssetKeys.h"

namespace Project::Assets {
  using Project::Utilities::LogsManager;
  using Project::Utilities::LuaStateWrapper;
  using Project::Handlers::ResourcesHandler;
  
  namespace Keys = Project::Libraries::Keys;
  TileAsset::TileAsset(SDL_Renderer* renderer, LogsManager& logsManager, ResourcesHandler& resourcesHandler)
    : BaseAsset(renderer, logsManager, resourcesHandler) {}
    
  TileAsset::~TileAsset() {
    if (data.texture) {
      SDL_DestroyTexture(data.texture);
      data.texture = nullptr;
    }
  }

  bool TileAsset::loadFromLua(const std::string& scriptPath, const std::string& assetName) {
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

    if (!assetName.empty()) {
      lua_pushstring(L, assetName.c_str());
    }

    if (lua_pcall(L, assetName.empty() ? 0 : 1, 1, 0) != LUA_OK) {
      const char* msg = lua_tostring(L, -1);
      logsManager.logError(std::string("Error executing getAsset: ") + (msg ? msg : "unknown"));
      return false;
    }

    if (!lua_istable(L, -1)) {
      logsManager.logError("getAsset must return a table");
      lua_pop(L, 1);
      return false;
    }
  }
}
