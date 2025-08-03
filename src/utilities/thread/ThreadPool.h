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
    static ThreadPool& getInstance();

    void enqueue(const std::function<void()>& job);
    void wait();

  private:
    ThreadPool();
    ~ThreadPool();

    void worker();

    LockFreeQueue<std::function<void()>> tasks;
    std::queue<std::function<void()>> taskQueue;
    std::vector<std::thread> workers;
    std::atomic<bool> running;
    std::mutex cvMutex;
    std::mutex taskMutex;
    std::condition_variable cv;
    std::atomic<size_t> active;
  };
}

#endif
