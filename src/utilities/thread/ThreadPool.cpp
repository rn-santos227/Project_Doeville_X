#include "ThreadPool.h"

#include <algorithm>
#include <exception>

namespace Project::Utilities {
  ThreadPool &ThreadPool::getInstance() {
    static ThreadPool instance;
    return instance;
  }

  ThreadPool::ThreadPool()
    : stop(false), active(0), pending(0), nextQueue(0), contention(0), logger(nullptr) {
    size_t count = std::max(2u, std::thread::hardware_concurrency());
    workers.reserve(count);
    taskQueues.resize(count);
    for (size_t i = 0; i < count; ++i) {
      workers.emplace_back(&ThreadPool::worker, this, i);
    }
  }

  ThreadPool::~ThreadPool() {
    if (logger) logger->logMessage("ThreadPool: shutting down");
    stop.store(true, std::memory_order_release);
    cv.notify_all();
    for (auto &t : workers) {
      if (t.joinable()) t.join();
    }
  }

  void ThreadPool::setLogger(Project::Utilities::LogsManager* log) {
    logger = log;
    if (logger) logger->logMessage("ThreadPool: logger attached with " + std::to_string(workers.size()) + " workers");
  }

  void ThreadPool::enqueue(std::function<void()> job) {
    if (!job || stop.load(std::memory_order_acquire)) return;
    tasks.push(std::move(job));
    cv.notify_one();
  }

  void ThreadPool::wait() {
    std::unique_lock<std::mutex> lock(cvMutex);
    cv.wait(lock, [this] {
      return tasks.empty() && active.load(std::memory_order_acquire) == 0;
    });
  }

  void ThreadPool::worker() {
    while (true) {
      std::function<void()> job;
      bool hasJob = false;
      
      hasJob = tasks.pop(job);
      
      if (!hasJob) {
        std::unique_lock<std::mutex> lock(cvMutex);
        cv.wait(lock, [this] {
          return stop.load(std::memory_order_acquire) || !tasks.empty();
        });
        
        if (stop.load(std::memory_order_acquire) && tasks.empty()) {
          if (logger) logger->logMessage("ThreadPool: worker exiting");
          return;
        }
        continue;
      }

      active.fetch_add(1, std::memory_order_acq_rel);
      try { 
        job();
      } catch (const std::exception &e) {
        if (logger) logger->logMessage(std::string("ThreadPool: task exception - ") + e.what());
      } catch (...) {
        if (logger) logger->logMessage("ThreadPool: task threw unknown exception");
      }
      active.fetch_sub(1, std::memory_order_acq_rel);
      
      if (tasks.empty() && active.load(std::memory_order_acquire) == 0) {
        std::lock_guard<std::mutex> lock(cvMutex);
        cv.notify_all();
      }
    }
  }
}
