#ifndef RESOURCESHANDLER_H
#define RESOURCESHANDLER_H

#include <string>
#include <SDL.h>

namespace Project::Handlers {
  class ResourcesHandler {
  public:
    static std::string getResourcePath(const std::string& relativePath);
    static SDL_Texture* loadTexture(SDL_Renderer* renderer, const std::string& imagePath);

  private:
    static std::string getBasePath();
  };
}
  
#endif