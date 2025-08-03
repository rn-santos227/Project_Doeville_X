#include "ThreadPool.h"

#include <algorithm>

namespace Project::Utilities {
  ThreadPool &ThreadPool::getInstance() {
    static ThreadPool instance;
    return instance;
  }

  ThreadPool::ThreadPool() : stop(false), active(0) {
    size_t count = std::max(2u, std::thread::hardware_concurrency());
    workers.reserve(count);
    for (size_t i = 0; i < count; ++i) {
      workers.emplace_back(&ThreadPool::worker, this);
    }
  }

  ThreadPool::~ThreadPool() {
    stop.store(true, std::memory_order_release);
    cv.notify_all();
    for (auto &t : workers) {
      if (t.joinable())
        t.join();
    }
  }

  void ThreadPool::enqueue(std::function<void()> job) {
    if (!job || stop.load(std::memory_order_acquire))
      return;
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
      if (!tasks.pop(job)) {
        std::unique_lock<std::mutex> lock(cvMutex);
        cv.wait(lock, [this] {
          return stop.load(std::memory_order_acquire) || !tasks.empty();
        });
        if (stop.load(std::memory_order_acquire) && tasks.empty())
          return;
        else
          continue;
      }
      active.fetch_add(1, std::memory_order_acq_rel);
      job();
      active.fetch_sub(1, std::memory_order_acq_rel);
      if (tasks.empty() && active.load(std::memory_order_acquire) == 0) {
        std::lock_guard<std::mutex> lock(cvMutex);
        cv.notify_all();
      }
    }
  }
}
