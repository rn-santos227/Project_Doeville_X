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