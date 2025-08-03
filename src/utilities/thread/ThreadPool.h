#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

#include "LockFreeQueue.h"

namespace Project::Utilities {
  class ThreadPool {
  public:
    static ThreadPool &getInstance();

    void enqueue(std::function<void()> job);
    void wait();

  private:
    ThreadPool();
    ~ThreadPool();

    void worker();

    std::vector<std::thread> workers;
    LockFreeQueue<std::function<void()>> tasks;
    std::condition_variable cv;
    std::mutex cvMutex;
    std::atomic<bool> stop;
    std::atomic<size_t> active;
  };
}

#endif
