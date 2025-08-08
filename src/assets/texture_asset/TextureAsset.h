#ifndef TEXTURE_ASSET_H
#define TEXTURE_ASSET_H

#include "TextureAssetData.h"

#include <string>
#include <SDL.h>

#include "assets/BaseAsset.h"
#include "utilities/lua_state_wrapper/LuaStateWrapper.h"


namespace Project::Assets {
  class TextureAsset : public BaseAsset {
  public:
    TextureAsset(
      SDL_Renderer* renderer,
      Project::Utilities::LogsManager& logsManager,
      Project::Handlers::ResourcesHandler& resourcesHandler
    );
    ~TextureAsset() override;

    bool loadFromLua(const std::string& scriptPath) override;

    SDL_Texture* getTexture() const { return texture; }
    int getWidth() const { return data.width; }
    int getHeight() const { return data.height; }
    float getScale() const { return data.scale; }
    
  private:
    SDL_Texture* texture;
    TextureAssetData data;
  };
}

#endif
