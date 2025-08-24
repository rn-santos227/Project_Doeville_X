#ifndef RESOURCESHANDLER_H
#define RESOURCESHANDLER_H

#include "BackgroundLoader.h"
#include "TextureAtlas.h"
#include "AsyncResourceLoader.h"
#include "StreamingManager.h"

#include <atomic>
#include <condition_variable>
#include <filesystem>
#include <future>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

#include <SDL.h>
#include <SDL_image.h>

#include "interfaces/cleanup_interface/Cleanable.h"
#include "utilities/logs_manager/LogsManager.h"
#include "utilities/memory/MemoryBudgetTracker.h"
#include "utilities/memory/PoolAllocator.h"
#include "watchers/hot_reload/HotReloadWatcher.h"

namespace Project::Handlers {
  class ResourcesHandler : public Project::Interfaces::Cleanable {
  public:
    explicit ResourcesHandler(Project::Utilities::LogsManager& logsManager);
    ~ResourcesHandler();

    void cleanup() override;

    std::string getResourcePath(const std::string& relativePath);
    
    std::future<SDL_Texture*> loadTextureAsync(SDL_Renderer* renderer, const std::string& imagePath);
    SDL_Texture* loadTexture(SDL_Renderer* renderer, const std::string& imagePath);
    SDL_Rect getTextureRegion(SDL_Renderer* renderer, const std::string& imagePath);

    std::future<MeshData> loadMeshAsync(const std::string& path);
    std::future<AudioData> loadAudioAsync(const std::string& path);

    std::vector<SDL_Texture*> sliceImage(SDL_Renderer* renderer, const std::string& imagePath, int frameWidth, int frameHeight);
    SDL_Texture* cropImage(SDL_Renderer* renderer, const std::string& imagePath, SDL_Rect cropRect);

  private:
    Project::Utilities::LogsManager& logsManager;
    Project::Utilities::MemoryBudgetTracker memoryTracker;
    Project::Utilities::PoolAllocator resourcePool;
    Project::Watchers::HotReloadWatcher hotReload;
    
    AsyncResourceLoader asyncLoader;
    BackgroundLoader backgroundLoader;
    StreamingManager streamingManager;
    TextureAtlas textureAtlas;

    std::unordered_map<SDL_Renderer*, SDL_Texture*> fallbackTextures;
    std::unordered_map<std::string, SDL_Texture*> textureCache;
    std::mutex fallbackMutex;
    std::mutex textureCacheMutex;
    
    SDL_Texture* getFallbackTexture(SDL_Renderer* renderer);
    std::string getBasePath();
  };
}

#endif
