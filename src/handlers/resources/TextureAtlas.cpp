#include "TextureAtlas.h"

#include "libraries/constants/NumericConstants.h"

namespace Project::Handlers {
  namespace Constants = Project::Libraries::Constants;
  TextureAtlas::~TextureAtlas() {
    for (auto& [renderer, data] : atlases) {
      if (data.texture) {
        SDL_DestroyTexture(data.texture);
        data.texture = nullptr;
      }
    }
  }

  SDL_Texture* TextureAtlas::ensureAtlas(SDL_Renderer* renderer) {
    auto it = atlases.find(renderer);
    if (it != atlases.end()) return it->second.texture;

    AtlasData data;
    data.texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, Constants::ATLAS_WIDTH, Constants::ATLAS_HEIGHT);
    if (!data.texture) return nullptr;
    atlases[renderer] = data;
    return data.texture;
  }

  SDL_Texture* TextureAtlas::addTexture(SDL_Renderer* renderer, SDL_Texture* source, int width, int height, const std::string& key) {
    if (!renderer || !source) return nullptr;

    std::lock_guard<std::mutex> lock(mutex);
    SDL_Texture* atlasTexture = ensureAtlas(renderer);
    if (!atlasTexture) return nullptr;

    AtlasData& data = atlases[renderer];
    if (data.nextX + width > Constants::ATLAS_WIDTH) {
      data.nextX = 0;
      data.nextY += data.rowHeight;
      data.rowHeight = 0;
    }
    if (data.nextY + height > Constants::ATLAS_HEIGHT) {
      return nullptr;
    }

    SDL_Rect dest{data.nextX, data.nextY, width, height};
    SDL_SetRenderTarget(renderer, atlasTexture);
    SDL_RenderCopy(renderer, source, nullptr, &dest);
    SDL_SetRenderTarget(renderer, nullptr);

    data.regions[key] = dest;
    data.nextX += width;
    if (height > data.rowHeight) data.rowHeight = height;

    return atlasTexture;
  }

  SDL_Rect TextureAtlas::getRegion(SDL_Renderer* renderer, const std::string& key) const {
    std::lock_guard<std::mutex> lock(mutex);
    auto it = atlases.find(renderer);
    if (it == atlases.end()) return SDL_Rect{0,0,0,0};
    const auto& regions = it->second.regions;
    auto rit = regions.find(key);
    if (rit == regions.end()) return SDL_Rect{0,0,0,0};
    return rit->second;
  }
}
