#include "AsyncResourceLoader.h"

#include <algorithm>

#include "utilities/memory/MemoryMappedFile.h"
#include "utilities/compression/CompressionUtils.h"

namespace Project::Handlers {
  using Project::Utilities::LogsManager;
  using Project::Utilities::PoolAllocator;

  AsyncResourceLoader::AsyncResourceLoader(LogsManager& logsManager, PoolAllocator& pool)
    : logsManager(logsManager), pool(pool) {
    threadCount = std::max(1u, std::thread::hardware_concurrency());
    for (size_t i = 0; i < threadCount; ++i) {
      workers.emplace_back(&AsyncResourceLoader::workerLoop, this);
    }
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
    for (auto& w : workers) {
      if (w.joinable()) {
        w.join();
      }
    }
    {
      std::lock_guard<std::mutex> lock(tasksMutex);
      while (!tasks.empty()) {
        tasks.front().promise.set_value(nullptr);
        tasks.pop();
      }
    }
  }

  SDL_Surface* AsyncResourceLoader::loadFromFile(const std::string& path) {
    Project::Utilities::MemoryMappedFile mmf(path);
    if (!mmf.isValid()) {
      logsManager.logError("Failed to memory map: " + path);
      return nullptr;
    }
    const unsigned char* data = mmf.data();
    size_t size = mmf.size();
    SDL_Surface* surface = nullptr;
    if (path.size() > 3 && path.substr(path.size() - 3) == ".gz") {
      void* block = pool.acquire();
      if (!block) {
        logsManager.logError("Failed to acquire buffer block for: " + path);
        return nullptr;
      }
      size_t outSize = 0;
      if (!Project::Utilities::CompressionUtils::decompressTo(data, size, static_cast<unsigned char*>(block), pool.getBlockSize(), outSize)) {
        logsManager.logError("Failed to decompress: " + path);
        pool.release(block);
        return nullptr;
      }
      SDL_RWops* rw = SDL_RWFromConstMem(block, static_cast<int>(outSize));
      surface = IMG_Load_RW(rw, 1);
      pool.release(block);
    } else {
      SDL_RWops* rw = SDL_RWFromConstMem(data, static_cast<int>(size));
      surface = IMG_Load_RW(rw, 1);
    }
    return surface;
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
      SDL_Surface* surface = loadFromFile(task.path);
      if (logsManager.checkAndLogError(!surface, "Failed to load image: " + task.path + " - " + IMG_GetError())) {
        task.promise.set_value(nullptr);
      } else {
        task.promise.set_value(surface);
        logsManager.logMessage("Loaded image: " + task.path);
      }
    }
  }
}
