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
  class AsyncResourceLoader {
  public:
    explicit AsyncResourceLoader(Project::Utilities::LogsManager& logsManager);
    ~AsyncResourceLoader();
  };
}

#endif
