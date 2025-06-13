#include "AsyncResourceLoader.h"

namespace Project::Handlers {
  AsyncResourceLoader::AsyncResourceLoader(LogsManager& logsManager)
    : logsManager(logsManager) {

  }

  std::future<SDL_Surface*> AsyncResourceLoader::loadSurface(const std::string& imagePath) {
    Task task;
    task.path = imagePath;
    std::future<SDL_Surface*> fut = task.promise.get_future();
  }
}
