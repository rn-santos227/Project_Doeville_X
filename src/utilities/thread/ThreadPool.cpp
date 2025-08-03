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

  void ThreadPool::enqueue(const std::function<void()>& job) {
    tasks.push(job);
    cv.notify_one();
  }

  void ThreadPool::wait() {
    while (true) {
      if (tasks.empty() && active.load(std::memory_order_acquire) == 0) {
        break;
      }
      std::function<void()> job;
      if (tasks.pop(job)) {
        active.fetch_add(1, std::memory_order_acq_rel);
        job();
        active.fetch_sub(1, std::memory_order_acq_rel);
        if (tasks.empty() && active.load(std::memory_order_acquire) == 0) {
          break;
        }
      } else {
        std::unique_lock<std::mutex> lock(cvMutex);
        cv.wait(lock);
      }
    }
  }

  void ThreadPool::worker() {
    while (running.load(std::memory_order_acquire)) {
      std::function<void()> job;
      if (tasks.pop(job)) {
        active.fetch_add(1, std::memory_order_acq_rel);
        job();
        active.fetch_sub(1, std::memory_order_acq_rel);
        if (tasks.empty() && active.load(std::memory_order_acquire) == 0) {
          cv.notify_all();
        }
      } else {
        std::unique_lock<std::mutex> lock(cvMutex);
        cv.wait(lock);
      }
    }
  }
}
