#ifndef STREAMING_MANAGER_H
#define STREAMING_MANAGER_H

#include <future>
#include <string>

#include <SDL.h>

#include "AsyncResourceLoader.h"
#include "BackgroundLoader.h"

namespace Project::Handlers {
  class StreamingManager {
  public:
    StreamingManager(AsyncResourceLoader& texLoader, BackgroundLoader& modelLoader);

    std::future<SDL_Texture*> requestTexture(SDL_Renderer* renderer, const std::string& path);
    std::future<MeshData> requestModel(const std::string& path);

  private:
    AsyncResourceLoader& textureLoader;
    BackgroundLoader& modelLoader;
  };
}

#endif
