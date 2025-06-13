#ifndef ASYNC_RESOURCE_LOADER_H
#define ASYNC_RESOURCE_LOADER_H

#include <atomic>
#include <condition_variable>
#include <future>
#include <mutex>
#include <queue>
#include <string>
#include <thread>

#include <SDL.h>
#include <SDL_image.h>

#include "utilities/logs_manager/LogsManager.h"

using namespace Project::Utilities;

namespace Project::Handlers {
  struct Task {
    std::string path;
    std::promise<SDL_Surface*> promise;
  };

  class AsyncResourceLoader {
  public:
    explicit AsyncResourceLoader(LogsManager& logsManager);
    ~AsyncResourceLoader();

    std::future<SDL_Surface*> loadSurface(const std::string& imagePath);
    void stop();

  private:
    LogsManager& logsManager;

    void workerLoop();

    std::queue<Task> tasks;
    std::mutex tasksMutex;
    std::condition_variable cv;
    std::thread worker;
    std::atomic<bool> running{true};
  };
}

#endif
