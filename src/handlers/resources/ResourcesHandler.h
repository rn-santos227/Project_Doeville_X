#ifndef RESOURCESHANDLER_H
#define RESOURCESHANDLER_H

#include <string>
#include <vector>
#include <SDL.h>
#include <SDL_image.h>

#include "utilities/logs_manager/LogsManager.h"

using namespace Project::Utilities;

namespace Project::Handlers {
  class ResourcesHandler {
  public:
    explicit ResourcesHandler(LogsManager& logsManager);
    
    std::string getResourcePath(const std::string& relativePath);
    SDL_Texture* loadTexture(SDL_Renderer* renderer, const std::string& imagePath);

    std::vector<SDL_Texture*> sliceImage(SDL_Renderer* renderer, const std::string& imagePath, int frameWidth, int frameHeight);
    SDL_Texture* cropImage(SDL_Renderer* renderer, const std::string& imagePath, SDL_Rect cropRect);

  private:
    LogsManager& logsManager; 

    std::string getBasePath();
  };
}
#endif
