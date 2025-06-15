#ifndef RESOURCESHANDLER_H
#define RESOURCESHANDLER_H

#include <string>
#include <unordered_map>
#include <future>
#include <mutex>
#include <vector>

#include <SDL.h>
#include <SDL_image.h>

#include "AsyncResourceLoader.h"

#include "utilities/logs_manager/LogsManager.h"

using namespace Project::Utilities;

namespace Project::Handlers {
  struct TextureTask {
    SDL_Renderer* renderer;
  };

  class ResourcesHandler {
  public:
    explicit ResourcesHandler(LogsManager& logsManager);
    ~ResourcesHandler();

    void cleanup();

    std::string getResourcePath(const std::string& relativePath);
    
    std::future<SDL_Texture*> loadTextureAsync(SDL_Renderer* renderer, const std::string& imagePath);
    SDL_Texture* loadTexture(SDL_Renderer* renderer, const std::string& imagePath);
    
    std::vector<SDL_Texture*> sliceImage(SDL_Renderer* renderer, const std::string& imagePath, int frameWidth, int frameHeight);
    SDL_Texture* cropImage(SDL_Renderer* renderer, const std::string& imagePath, SDL_Rect cropRect);

  private:
    LogsManager& logsManager; 
    AsyncResourceLoader asyncLoader;

    std::unordered_map<std::string, SDL_Texture*> textureCache;
    std::mutex textureCacheMutex;
    
    std::string getBasePath();
  };
}
#endif
