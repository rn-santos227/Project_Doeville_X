#ifndef TEXTURE_ASSET_H
#define TEXTURE_ASSET_H

#include "TextureAssetData.h"

#include <optional>
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

    SDL_Texture* getTexture() const { return data.texture; }
    int getWidth() const { return textureData.width; }
    int getHeight() const { return textureData.height; }
    float getScale() const { return textureData.scale; }

    std::optional<SDL_Color> getColor() const { return textureData.color; }
    bool setColor(SDL_Color color);
    bool setColorHex(const std::string& hex);
    
  private:
    TextureAssetData textureData;
  };
}

#endif
