#include "TextureAsset.h"

#include "libraries/keys/LuaAssetKeys.h"
#include "utilities/color/ColorUtils.h"

namespace Project::Assets {
  using Project::Utilities::LogsManager;
  using Project::Utilities::LuaStateWrapper;
  using Project::Utilities::ColorUtils;
  using Project::Handlers::ResourcesHandler;
  
  namespace Keys = Project::Libraries::Keys;
  TextureAsset::TextureAsset(SDL_Renderer* renderer, LogsManager& logsManager, ResourcesHandler& resourcesHandler)
    : BaseAsset(renderer, logsManager, resourcesHandler) {}

  TextureAsset::~TextureAsset() {
    if (data.texture) {
      SDL_DestroyTexture(data.texture);
      data.texture = nullptr;
    }
  }

  bool TextureAsset::loadFromLua(const std::string& scriptPath, const std::string& assetName) {
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

    lua_getfield(L, -1, Keys::LUA_ASSET_NAME);
    if (lua_isstring(L, -1)) data.name = lua_tostring(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, -1, Keys::LUA_ASSET_PATH);
    if (lua_isstring(L, -1)) data.path = lua_tostring(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, -1, Keys::LUA_ASSET_TAG);
    if (lua_isstring(L, -1)) data.tag = lua_tostring(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, -1, Keys::LUA_ASSET_WIDTH);
    if (lua_isnumber(L, -1)) textureData.width = static_cast<int>(lua_tointeger(L, -1));
    lua_pop(L, 1);

    lua_getfield(L, -1, Keys::LUA_ASSET_HEIGHT);
    if (lua_isnumber(L, -1)) textureData.height = static_cast<int>(lua_tointeger(L, -1));
    lua_pop(L, 1);

    lua_getfield(L, -1, Keys::LUA_ASSET_SCALE);
    if (lua_isnumber(L, -1)) textureData.scale = static_cast<float>(lua_tonumber(L, -1));
    lua_pop(L, 1);

    lua_getfield(L, -1, Keys::LUA_ASSET_COLOR);
    if (lua_isstring(L, -1)) textureData.color = ColorUtils::hexToRGB(lua_tostring(L, -1));
    lua_pop(L, 1);

    lua_pop(L, 1);

    data.category = AssetCategory::TEXTURE;

    if (data.path.empty()) {
      logsManager.logError("TextureAsset path is empty: " + scriptPath);
      return false;
    }

    data.texture = resourcesHandler.loadTexture(renderer, data.path);
    if (!data.texture) {
      logsManager.logError("Failed to load texture: " + data.path);
      return false;
    }

    if (textureData.width == 0 || textureData.height == 0) {
      SDL_QueryTexture(data.texture, nullptr, nullptr, &textureData.width, &textureData.height);
    }

    if (textureData.color) {
      setColor(*textureData.color);
    }

    return true;
  }

  bool TextureAsset::setColor(SDL_Color color) {
    if (!data.texture) {
      return false;
    }

    if (!ColorUtils::applyShader(data.texture, color)) {
      return false;
    }

    textureData.color = color;
    return true;
  }
  
  bool TextureAsset::setColorHex(const std::string& hex) {
    return setColor(ColorUtils::hexToRGB(hex));
  }
}
