#include "StreamingManager.h"

#include <utility>

namespace Project::Handlers {
  StreamingManager::StreamingManager(AsyncResourceLoader& texLoader, BackgroundLoader& modelLoader)
    : textureLoader(texLoader), modelLoader(modelLoader) {}

  std::future<SDL_Texture*> StreamingManager::requestTexture(SDL_Renderer* renderer, const std::string& path) {
    auto surfaceFuture = textureLoader.loadSurface(path);
    return std::async(std::launch::async, [renderer, fut = std::move(surfaceFuture)]() mutable {
      SDL_Surface* surface = fut.get();
      if (!surface) {
        return static_cast<SDL_Texture*>(nullptr);
      }
      SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surface);
      SDL_FreeSurface(surface);
      return tex;
    });
  }

  std::future<MeshData> StreamingManager::requestModel(const std::string& path) {
    return modelLoader.streamMesh(path);
  }
}
