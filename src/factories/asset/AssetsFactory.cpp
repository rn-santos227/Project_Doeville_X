#include "AssetsFactory.h"

#include "assets/BaseAsset.h"
#include "assets/map_asset/MapAsset.h"
#include "assets/texture_asset/TextureAsset.h"
#include "assets/tile_asset/TileAsset.h"
#include "assets/AssetCategoryResolver.h"
#include "libraries/keys/LuaAssetKeys.h"
#include "utilities/lua_state_wrapper/LuaStateWrapper.h"

namespace Project::Factories {
  using Project::Assets::BaseAsset;
  using Project::Assets::MapAsset;
  using Project::Assets::TextureAsset;
  using Project::Assets::TileAsset;
  using Project::Assets::AssetsManager;
  using Project::Assets::AssetCategory;
  using Project::Assets::AssetCategoryResolver;
  using Project::Handlers::ResourcesHandler;
  using Project::Utilities::LogsManager;
  using Project::Utilities::LuaStateWrapper;

  namespace Keys = Project::Libraries::Keys;

  AssetsFactory::AssetsFactory(
    SDL_Renderer* renderer,
    LogsManager& logsManager,
    ResourcesHandler& resourcesHandler,
    AssetsManager& assetsManager)
    : renderer(renderer),
      logsManager(logsManager),
      resourcesHandler(resourcesHandler),
      assetsManager(assetsManager) {}

  bool AssetsFactory::createAssetFromLua(const std::string &scriptPath, const std::string& assetName) {
    LuaStateWrapper lua(logsManager);
    if (!lua.loadScript(scriptPath)) {
      logsManager.logError("Failed to load asset script: " + scriptPath);
      return false;
    }

    lua_State* L = lua.get();
    lua_getglobal(L, Keys::LUA_ASSET_GET_ASSET);
    if (!lua_isfunction(L, -1)) {
      logsManager.logError("Asset script missing get_asset function: " + scriptPath);
      return false;
    }

    if (!assetName.empty()) {
      lua_pushstring(L, assetName.c_str());
    }

    if (lua_pcall(L, assetName.empty() ? 0 : 1, 1, 0) != LUA_OK) {
      const char* msg = lua_tostring(L, -1);
      logsManager.logError(std::string("Error executing get_asset: ") + (msg ? msg : Keys::LUA_ASSET_UNKNOWN));
      return false;
    }

    if (!lua_istable(L, -1)) {
      logsManager.logError("get_asset must return a table");
      lua_pop(L, 1);
      return false;
    }

    AssetCategory category = AssetCategory::TEXTURE;
    lua_getfield(L, -1, Keys::LUA_ASSET_CATEGORY);
    if (!lua_isstring(L, -1)) {
      lua_pop(L, 1);
      lua_pushnil(L);
      while (lua_next(L, -2) != 0) {
        if (lua_isstring(L, -2)) {
          std::string nestedName = lua_tostring(L, -2);
          if (!createAssetFromLua(scriptPath, nestedName)) {
            lua_pop(L, 1);
            lua_pop(L, 1);
            return false;
          }
        }
        lua_pop(L, 1);
      }
      lua_pop(L, 1);
      return true;
    }
    category = AssetCategoryResolver::resolve(lua_tostring(L, -1));
    
    lua_pop(L, 1);
    lua_pop(L, 1);

    std::unique_ptr<BaseAsset> asset;
    switch (category) {
      case AssetCategory::MAP:
        asset = std::make_unique<MapAsset>(renderer, logsManager, resourcesHandler);
        break;

      case AssetCategory::TEXTURE:
        asset = std::make_unique<TextureAsset>(renderer, logsManager, resourcesHandler);
        break;

      case AssetCategory::TILE:
        asset = std::make_unique<TileAsset>(renderer, logsManager, resourcesHandler);
        break;

      default:
        logsManager.logError("No available category.");
        return false;
    }

    if (!asset || !asset->loadFromLua(scriptPath, assetName)) {
      logsManager.logError("Failed to load asset from Lua: " + scriptPath);
      return false;
    }

    std::string name = asset->getName();
    if (name.empty()) {
      logsManager.logWarning("Asset name is empty after loading: " + scriptPath);
      return false;
    }

    assetsManager.addAsset(name, std::move(asset));
    return true;
  }
}
