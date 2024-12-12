#ifndef RESOURCESHANDLER_H
#define RESOURCESHANDLER_H

#include <string>
#include <SDL.h>

namespace Project::Handlers {
  class ResourcesHandler {
  public:
    static std::string getResourcePath(const std::string& relativePath);

  private:
    static std::string getBasePath();
  };

  #endif
}