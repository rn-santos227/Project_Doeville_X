#include "ResourcesHandler.h"
#include "AsyncResourceLoader.h"

#include "helpers/resource_cleaner/ResourceCleaner.h"
#include "libraries/constants/Constants.h"

namespace Project::Handlers {
  using Project::Utilities::LogsManager;
  using Project::Helpers::ResourceCleaner;

  namespace Constants = Project::Libraries::Constants;

  ResourcesHandler::ResourcesHandler(LogsManager& logsManager)
    : logsManager(logsManager), asyncLoader(logsManager) {
      workerThread = std::thread(&ResourcesHandler::workerLoop, this);
    }

  ResourcesHandler::~ResourcesHandler() {
    cleanup();
  }

  void ResourcesHandler::cleanup() {
    stopWorker();
    asyncLoader.stop();

    {
      std::lock_guard<std::mutex> lock(textureCacheMutex);
      ResourceCleaner::cleanupMap(textureCache, [](SDL_Texture* texture) {
        if (texture) {
          SDL_DestroyTexture(texture);
        }
      });
    }

    {
      std::lock_guard<std::mutex> lock(fallbackMutex);
      ResourceCleaner::cleanupMap(fallbackTextures, [](SDL_Texture* tex) {
        if (tex) {
          SDL_DestroyTexture(tex);
        }
      });
    }
  }

  std::string ResourcesHandler::getResourcePath(const std::string& relativePath) {
    namespace fs = std::filesystem;

    if (relativePath.empty() || relativePath[0] == '/' || relativePath.find("..") != std::string::npos) {
      logsManager.logError("Invalid resource path: " + relativePath);
      return "";
    }

    std::string basePath = getBasePath();
    if (!basePath.empty() && basePath.back() == '\\') {
      basePath.pop_back();
    }

    fs::path resourceDir = fs::weakly_canonical(fs::path(basePath) / Constants::DEFAULT_RESOURCES_FOLDER);
    fs::path absPath = fs::weakly_canonical(fs::path(basePath) / relativePath);

    std::string resStr = resourceDir.string();
    std::string absStr = absPath.string();

    if (absStr.rfind(resStr, 0) != 0) {
      logsManager.logError("Resource path outside resources directory: " + relativePath);
      return "";
    }

    return absStr;
  }

  SDL_Texture* ResourcesHandler::loadTexture(SDL_Renderer* renderer, const std::string& imagePath) {
    {
      std::lock_guard<std::mutex> lock(textureCacheMutex);
      auto it = textureCache.find(imagePath);
      if (it != textureCache.end()) {
        return it->second;
      }
    }

    SDL_Surface* surface = IMG_Load(imagePath.c_str());
    if (logsManager.checkAndLogError(!surface, "Failed to load image: " + imagePath + " - " + IMG_GetError())) {
      SDL_Texture* fallback = getFallbackTexture(renderer);
      {
        std::lock_guard<std::mutex> lock(textureCacheMutex);
        textureCache[imagePath] = fallback;
      }
      return fallback;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    if (logsManager.checkAndLogError(!texture, "Failed to create texture from image: " + imagePath + " - " + SDL_GetError())) {
      SDL_Texture* fallback = getFallbackTexture(renderer);
      {
        std::lock_guard<std::mutex> lock(textureCacheMutex);
        textureCache[imagePath] = fallback;
      }
      return fallback;
    }

    {
      std::lock_guard<std::mutex> lock(textureCacheMutex);
      textureCache[imagePath] = texture;
    }
    return texture;
  }

  std::future<SDL_Texture*> ResourcesHandler::loadTextureAsync(SDL_Renderer* renderer, const std::string& imagePath) {
    {
      std::lock_guard<std::mutex> lock(textureCacheMutex);
      auto it = textureCache.find(imagePath);
      if (it != textureCache.end()) {
        std::promise<SDL_Texture*> p;
        p.set_value(it->second);
        return p.get_future();
      }
    }

    TextureTask task;
    task.renderer = renderer;
    task.path = imagePath;
    std::future<SDL_Texture*> fut = task.promise.get_future();

    {
      std::lock_guard<std::mutex> lock(tasksMutex);
      textureTasks.push(std::move(task));
    }
    tasksCv.notify_one();
    return fut;
  }

  SDL_Texture* ResourcesHandler::cropImage(SDL_Renderer* renderer, const std::string& imagePath, SDL_Rect cropRect) {
    SDL_Texture* fullTexture = loadTexture(renderer, imagePath);
    if (!fullTexture) return nullptr;

    int texWidth, texHeight;
    SDL_QueryTexture(fullTexture, nullptr, nullptr, &texWidth, &texHeight);

    if (cropRect.x < 0 || cropRect.y < 0 || cropRect.w <= 0 || cropRect.h <= 0 || cropRect.x + cropRect.w > texWidth || cropRect.y + cropRect.h > texHeight) {
      logsManager.logWarning("Invalid crop dimensions: (" + 
        std::to_string(cropRect.x) + "," + std::to_string(cropRect.y) + "," + 
        std::to_string(cropRect.w) + "," + std::to_string(cropRect.h) + 
        ") (Image size: " + std::to_string(texWidth) + "x" + std::to_string(texHeight) + ")");
    
      return nullptr;
    }

    SDL_Texture* croppedTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, cropRect.w, cropRect.h);
    SDL_SetRenderTarget(renderer, croppedTexture);
    SDL_RenderCopy(renderer, fullTexture, &cropRect, nullptr);
    SDL_SetRenderTarget(renderer, nullptr);

    return croppedTexture;
  }

  std::vector<SDL_Texture*> ResourcesHandler::sliceImage(SDL_Renderer* renderer, const std::string& imagePath, int frameWidth, int frameHeight) {
    std::vector<SDL_Texture*> frames;
    SDL_Texture* fullTexture = loadTexture(renderer, imagePath);

    if (!fullTexture) return frames;

    int texWidth, texHeight;
    SDL_QueryTexture(fullTexture, nullptr, nullptr, &texWidth, &texHeight);

    if (frameWidth <= 0 || frameHeight <= 0 || frameWidth > texWidth || frameHeight > texHeight) {
      logsManager.logWarning("Invalid frame size for slicing: " + std::to_string(frameWidth) + "x" + std::to_string(frameHeight) +
      " (Image size: " + std::to_string(texWidth) + "x" + std::to_string(texHeight) + ")");
      return frames;
    }

    const int totalFramesX = texWidth / frameWidth;
    const int totalFramesY = texHeight / frameHeight;
    frames.reserve(totalFramesX * totalFramesY);

    for (int y = 0; y < texHeight; y += frameHeight) {
      for (int x = 0; x < texWidth; x += frameWidth) {
        SDL_Rect srcRect = {x, y, frameWidth, frameHeight};

        if (x + frameWidth > texWidth || y + frameHeight > texHeight) continue;

        SDL_Texture* cropped = cropImage(renderer, imagePath, srcRect);
        if (cropped) frames.push_back(cropped);
      }
    }
    return frames;
  }

  void ResourcesHandler::stopWorker() {
    running = false;
    tasksCv.notify_all();
    if (workerThread.joinable()) {
      workerThread.join();
    }
  }

  void ResourcesHandler::workerLoop() {
    while (running) {
      TextureTask task;
      {
        std::unique_lock<std::mutex> lock(tasksMutex);
        tasksCv.wait(lock, [this] { return !textureTasks.empty() || !running; });
        if (!running && textureTasks.empty()) {
          return;
        }
        task = std::move(textureTasks.front());
        textureTasks.pop();
      }

      auto surfaceFuture = asyncLoader.loadSurface(task.path);
      SDL_Surface* surface = surfaceFuture.get();
      if (logsManager.checkAndLogError(!surface, "Failed to load image asynchronously: " + task.path + " - " + IMG_GetError())) {
        SDL_Texture* fallback = getFallbackTexture(task.renderer);
        {
          std::lock_guard<std::mutex> lock(textureCacheMutex);
          textureCache[task.path] = fallback;
        }
        task.promise.set_value(fallback);
        continue;
      }

      SDL_Texture* texture = SDL_CreateTextureFromSurface(task.renderer, surface);
      SDL_FreeSurface(surface);
      if (logsManager.checkAndLogError(!texture, "Failed to create texture from image asynchronously: " + task.path + " - " + SDL_GetError())) {
        SDL_Texture* fallback = getFallbackTexture(task.renderer);
        {
          std::lock_guard<std::mutex> lock(textureCacheMutex);
          textureCache[task.path] = fallback;
        }
        task.promise.set_value(fallback);
        continue;
      }

      {
        std::lock_guard<std::mutex> lock(textureCacheMutex);
        textureCache[task.path] = texture;
      }
      task.promise.set_value(texture);
    }
  }

  SDL_Texture* ResourcesHandler::getFallbackTexture(SDL_Renderer* renderer) {
    std::lock_guard<std::mutex> lock(fallbackMutex);
    auto it = fallbackTextures.find(renderer);
    if (it != fallbackTextures.end()) {
      return it->second;
    }

    SDL_Surface* surface = SDL_CreateRGBSurfaceWithFormat(0, Constants::INDEX_TWO, Constants::INDEX_TWO, Constants::BIT_32, SDL_PIXELFORMAT_RGBA32);
    if (!surface) {
      logsManager.logError(std::string("Failed to create fallback surface: ") + SDL_GetError());
      return nullptr;
    }

    Uint32 magenta = SDL_MapRGBA(surface->format, Constants::BIT_255, 0, Constants::BIT_255, Constants::BIT_255);
    Uint32 black = SDL_MapRGBA(surface->format, 0, 0, 0, Constants::BIT_255);
    Uint32* pixels = static_cast<Uint32*>(surface->pixels);
    pixels[Constants::INDEX_ZERO] = magenta;
    pixels[Constants::INDEX_ONE] = black;
    pixels[Constants::INDEX_TWO] = black;
    pixels[Constants::INDEX_THREE] = magenta;

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    if (!texture) {
      logsManager.logError(std::string("Failed to create fallback texture: ") + SDL_GetError());
      return nullptr;
    }

    fallbackTextures[renderer] = texture;
    return texture;
  }

  std::string ResourcesHandler::getBasePath() {
    char* basePath = SDL_GetBasePath();
    std::string path = basePath ? basePath : Constants::EMPTY_STRING;
    SDL_free(basePath);
    return path;
  }
}
