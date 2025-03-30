#include "ResourcesHandler.h"

namespace Project::Handlers {
  ResourcesHandler::ResourcesHandler(Project::Utilities::LogsManager& logsManager)
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
  }
}