#include "ResourcesHandler.h"

namespace Project::Handlers {
  ResourcesHandler::ResourcesHandler(LogsManager& logsManager)
    : logsManager(logsManager) {}

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
    SDL_Surface* surface = IMG_Load(imagePath.c_str());
    
    if (logsManager.checkAndLogError(!surface, "Failed to load image: " + imagePath + " - " + IMG_GetError())) {
      logsManager.flushLogs();
      return nullptr;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    if (logsManager.checkAndLogError(!texture, "Failed to create texture from image: " + imagePath + " - " + SDL_GetError())) {
      logsManager.flushLogs();
      return nullptr;
    }

    return texture;
  }

  std::vector<SDL_Texture*> ResourcesHandler::sliceImage(SDL_Renderer* renderer, const std::string& imagePath, int frameWidth, int frameHeight) {
    std::vector<SDL_Texture*> frames;
    SDL_Texture* fullTexture = loadTexture(renderer, imagePath);

    if (!fullTexture) return frames;

    int texWidth, texHeight;
    SDL_QueryTexture(fullTexture, nullptr, nullptr, &texWidth, &texHeight);

    if (frameWidth <= 0 || frameHeight <= 0 || frameWidth > texWidth || frameHeight > texHeight) {
      logsManager.checkAndLogError(true, "Invalid frame size for slicing: " + std::to_string(frameWidth) + "x" + std::to_string(frameHeight) +
      " (Image size: " + std::to_string(texWidth) + "x" + std::to_string(texHeight) + ")");

      logsManager.flushLogs();
      SDL_DestroyTexture(fullTexture);
      return frames;
    }
  }
}