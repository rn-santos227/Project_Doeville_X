#ifndef RESOURCESHANDLER_H
#define RESOURCESHANDLER_H

#include <atomic>
#include <condition_variable>
#include <filesystem>
#include <future>
#include <mutex>
#include <queue>
#include <string>
#include <unordered_map>
#include <vector>

#include <SDL.h>
#include <SDL_image.h>

#include "AsyncResourceLoader.h"

#include "interfaces/cleanup_interface/Cleanable.h"
#include "utilities/logs_manager/LogsManager.h"

namespace Project::Handlers {
  struct TextureTask {
    SDL_Renderer* renderer;
    std::string path;
    std::promise<SDL_Texture*> promise;
  };

  class ResourcesHandler : public Project::Interfaces::Cleanable {
  public:
    explicit ResourcesHandler(Project::Utilities::LogsManager& logsManager);
    ~ResourcesHandler();

    void cleanup() override;

    std::string getResourcePath(const std::string& relativePath);
    
    std::future<SDL_Texture*> loadTextureAsync(SDL_Renderer* renderer, const std::string& imagePath);
    SDL_Texture* loadTexture(SDL_Renderer* renderer, const std::string& imagePath);
    
    std::vector<SDL_Texture*> sliceImage(SDL_Renderer* renderer, const std::string& imagePath, int frameWidth, int frameHeight);
    SDL_Texture* cropImage(SDL_Renderer* renderer, const std::string& imagePath, SDL_Rect cropRect);

  private:
    Project::Utilities::LogsManager& logsManager;
    AsyncResourceLoader asyncLoader;
    

    std::queue<TextureTask> textureTasks;
    std::mutex tasksMutex;
    std::condition_variable tasksCv;
    std::thread workerThread;
    std::atomic<bool> running{true};

    std::unordered_map<SDL_Renderer*, SDL_Texture*> fallbackTextures;
    std::unordered_map<std::string, SDL_Texture*> textureCache;
    std::mutex fallbackMutex;
    std::mutex textureCacheMutex;

    void stopWorker();
    void workerLoop();    
    
    SDL_Texture* getFallbackTexture(SDL_Renderer* renderer);
    std::string getBasePath();
  };
}

#endif
