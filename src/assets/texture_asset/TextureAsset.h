#ifndef TEXTURE_ASSET_H
#define TEXTURE_ASSET_H

#include "TextureAssetData.h"

#include <string>
#include <SDL.h>

#include "assets/BaseAsset.h"
#include "libraries/constants/NameConstants.h"
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

    bool loadFromLua(
      const std::string& scriptPath,
      const std::string& assetName = Project::Libraries::Constants::EMPTY_STRING
    ) override;

    SDL_Texture* getTexture() const { return texture; }
    int getWidth() const { return textureData.width; }
    int getHeight() const { return textureData.height; }
    float getScale() const { return textureData.scale; }

    SDL_Color getColor() const { return textureData.color; }
    bool setColor(SDL_Color color);
    
  private:
    SDL_Texture* texture;
    TextureAssetData textureData;
  };
}

#endif
