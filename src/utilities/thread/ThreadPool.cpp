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
    size_t count = std::max<size_t>(2u, std::thread::hardware_concurrency());
    taskQueues = std::vector<decltype(taskQueues)::value_type>(count);
    workers.reserve(count);

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
    size_t index = nextQueue.fetch_add(1, std::memory_order_relaxed) % taskQueues.size();
    taskQueues[index].push(std::move(job));
    pending.fetch_add(1, std::memory_order_release);
    std::lock_guard<std::mutex> lock(cvMutex);
    cv.notify_all();
  }

  void ThreadPool::wait() {
    std::unique_lock<std::mutex> lock(cvMutex);
    cv.wait(lock, [this] {
      return pending.load(std::memory_order_acquire) == 0 && active.load(std::memory_order_acquire) == 0;
    });
    contention.store(0, std::memory_order_release);
  }

  void ThreadPool::worker(size_t index) {
    while (true) {
      std::function<void()> job;
      bool hasJob = taskQueues[index].pop(job);

      if (!hasJob) {
        for (size_t i = 0; i < taskQueues.size(); ++i) {
          if (i == index) continue;
          if (taskQueues[i].pop(job)) {
            hasJob = true;
            break;
          }
        }
      }
      
      if (!hasJob) {
        contention.fetch_add(1, std::memory_order_relaxed);
        std::unique_lock<std::mutex> lock(cvMutex);
        cv.wait(lock, [this] {
          return stop.load(std::memory_order_acquire) || pending.load(std::memory_order_acquire) > 0;
        });
        
        if (stop.load(std::memory_order_acquire) && pending.load(std::memory_order_acquire) == 0) {
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
      pending.fetch_sub(1, std::memory_order_acq_rel);

      if (pending.load(std::memory_order_acquire) == 0 &&
          active.load(std::memory_order_acquire) == 0) {
        std::lock_guard<std::mutex> lock(cvMutex);
        cv.notify_all();
      }
    }
  }
}
