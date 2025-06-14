#include "ResourcesHandler.h"
#include "AsyncResourceLoader.h"

namespace Project::Handlers {
  ResourcesHandler::ResourcesHandler(LogsManager& logsManager)
    : logsManager(logsManager), asyncLoader(logsManager) {}

  ResourcesHandler::~ResourcesHandler() {
    cleanup();
  }

  void ResourcesHandler::cleanup() {
    std::lock_guard<std::mutex> lock(textureCacheMutex);
    for (auto& pair : textureCache) {
      if (pair.second) {
        SDL_DestroyTexture(pair.second);
      }
    }
    textureCache.clear();
  }

  std::string ResourcesHandler::getBasePath() {
    char* basePath = SDL_GetBasePath();
    std::string path = basePath ? basePath : "";
    SDL_free(basePath);
    return path;
  }

  std::string ResourcesHandler::getResourcePath(const std::string& relativePath) {
    std::string basePath = getBasePath();

    if (basePath.back() == '\\') {
      basePath.pop_back();
    }

    return basePath + "/" + relativePath;
  }

  SDL_Texture* ResourcesHandler::loadTexture(SDL_Renderer* renderer, const std::string& imagePath) {
    {
      std::lock_guard<std::mutex> lock(textureCacheMutex);
      auto it = textureCache.find(imagePath);
    }

    SDL_Surface* surface = IMG_Load(imagePath.c_str());
    if (logsManager.checkAndLogError(!surface, "Failed to load image: " + imagePath + " - " + IMG_GetError())) {
      return nullptr;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    if (logsManager.checkAndLogError(!texture, "Failed to create texture from image: " + imagePath + " - " + SDL_GetError())) {
      return nullptr;
    }

    textureCache[imagePath] = texture;
    return texture;
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
}
