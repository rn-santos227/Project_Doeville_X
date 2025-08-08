#include "TextureAsset.h"

#include "libraries/keys/LuaAssetKeys.h"
#include "utilities/color_shader/ColorShader.h"

namespace Project::Assets {
  using Project::Utilities::LogsManager;
  using Project::Utilities::LuaStateWrapper;
  using Project::Handlers::ResourcesHandler;
  using Project::Libraries::Keys::LUA_ASSET_GET_ASSET;
  
  namespace Keys = Project::Libraries::Keys;

  TextureAsset::TextureAsset(SDL_Renderer* renderer, LogsManager& logsManager, ResourcesHandler& resourcesHandler)
    : BaseAsset(renderer, logsManager, resourcesHandler),
      texture(nullptr) {}

}