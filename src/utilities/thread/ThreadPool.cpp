#include "ThreadPool.h"

#include <algorithm>
#include <cassert>

namespace Project::Utilities {
  ThreadPool& ThreadPool::getInstance() {
    static ThreadPool instance;
    return instance;
  }

  ThreadPool::ThreadPool() : running(true), active(0) {
    size_t count = std::max(2u, std::thread::hardware_concurrency());
    workers.reserve(count);
    for (size_t i = 0; i < count; ++i) {
      workers.emplace_back(&ThreadPool::worker, this);
    }
  }

  ThreadPool::~ThreadPool() {
    running.store(false, std::memory_order_release);
    cv.notify_all();
    for (auto& t : workers) {
      if (t.joinable()) t.join();
    }
  }

  void ThreadPool::enqueue(std::function<void()> job) {
    if (!job || !running.load(std::memory_order_acquire)) return;
    {
      std::lock_guard<std::mutex> lock(taskMutex);
      taskQueue.emplace(std::move(job));
    }
    cv.notify_one();
  }

  void ThreadPool::wait() {
    std::unique_lock<std::mutex> lock(taskMutex);
    cv.wait(lock, [this] {
      return taskQueue.empty() && active.load(std::memory_order_acquire) == 0;
    });
  }

  void ThreadPool::worker() {
    while (running.load(std::memory_order_acquire)) {
      std::function<void()> job;
      {
        std::unique_lock<std::mutex> lock(taskMutex);
        cv.wait(lock, [this] {
          return !taskQueue.empty() || !running.load(std::memory_order_acquire);
        });

        if (!running.load() && taskQueue.empty())
          return;

        job = taskQueue.front();
        taskQueue.pop();
        active.fetch_add(1, std::memory_order_acq_rel);
      }

      job();

      active.fetch_sub(1, std::memory_order_acq_rel);

      {
        std::unique_lock<std::mutex> lock(taskMutex);
        if (taskQueue.empty() && active.load(std::memory_order_acquire) == 0) {
          cv.notify_all();  // Notify any waiters
        }
      }
    }
  }
}
