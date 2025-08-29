#ifndef TEXTURE_ATLAS_H
#define TEXTURE_ATLAS_H

#include <unordered_map>
#include <string>
#include <mutex>

#include <SDL.h>

namespace Project::Handlers {
  class TextureAtlas {
  public:
    TextureAtlas() = default;
    ~TextureAtlas();

    SDL_Texture* addTexture(SDL_Renderer* renderer, SDL_Texture* source, int width, int height, const std::string& key);
    SDL_Rect getRegion(SDL_Renderer* renderer, const std::string& key) const;

  private:
    struct AtlasData {
      SDL_Texture* texture = nullptr;
      int nextX = 0;
      int nextY = 0;
      int rowHeight = 0;
      std::unordered_map<std::string, SDL_Rect> regions;
    };
    
    std::unordered_map<SDL_Renderer*, AtlasData> atlases;
    mutable std::mutex mutex;

    SDL_Texture* ensureAtlas(SDL_Renderer* renderer);
  };
}

#endif
