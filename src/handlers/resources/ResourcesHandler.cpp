#include "ResourcesHandler.h"
#include <iostream>
#include <SDL2/SDL.h>
#include <cstdlib> 

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