#include "AsyncResourceLoader.h"

namespace Project::Handlers {
  using Project::Utilities::LogsManager;

  AsyncResourceLoader::AsyncResourceLoader(LogsManager& logsManager)
    : logsManager(logsManager) {
    worker = std::thread(&AsyncResourceLoader::workerLoop, this);
  }

  AsyncResourceLoader::~AsyncResourceLoader() {
    stop();
  }

  std::future<SDL_Surface*> AsyncResourceLoader::loadSurface(const std::string& imagePath) {
    Task task;
    task.path = imagePath;
    std::future<SDL_Surface*> fut = task.promise.get_future();
    {
      std::lock_guard<std::mutex> lock(tasksMutex);
      tasks.push(std::move(task));
    }
    cv.notify_one();
    return fut;
  }

  void AsyncResourceLoader::stop() {
    running = false;
    cv.notify_all();
    if (worker.joinable()) {
      worker.join();
    }
    {
      std::lock_guard<std::mutex> lock(tasksMutex);
      while (!tasks.empty()) {
        tasks.front().promise.set_value(nullptr);
        tasks.pop();
      }
    }
  }

  void AsyncResourceLoader::workerLoop() {
    while (running) {
      Task task;
      {
        std::unique_lock<std::mutex> lock(tasksMutex);
        cv.wait(lock, [this] { return !tasks.empty() || !running; });
        if (!running && tasks.empty()) {
          return;
        }
        task = std::move(tasks.front());
        tasks.pop();
      }
      SDL_Surface* surface = IMG_Load(task.path.c_str());
      if (logsManager.checkAndLogError(!surface, "Failed to load image: " + task.path + " - " + IMG_GetError())) {
        task.promise.set_value(nullptr);
      } else {
        task.promise.set_value(surface);
        logsManager.logMessage("Loaded image: " + task.path);
      }
    }
  }
}
