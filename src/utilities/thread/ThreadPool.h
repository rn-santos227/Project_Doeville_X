#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include "LockFreeQueue.h"

#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <thread>
#include <vector>

#include "utilities/logs_manager/LogsManager.h"

namespace Project::Utilities {
  class ThreadPool {
  public:
    static ThreadPool &getInstance();

    void setLogger(Project::Utilities::LogsManager* logger);
    void enqueue(std::function<void()> job);
    void wait();

  private:
    ThreadPool();
    ~ThreadPool();

    Project::Utilities::LogsManager* logger;

    std::vector<std::thread> workers;
    std::vector<LockFreeQueue<std::function<void()>>> taskQueues;
    std::condition_variable cv;
    std::mutex cvMutex;
    std::atomic<bool> stop;
    std::atomic<size_t> active;
    std::atomic<size_t> pending;
    std::atomic<size_t> nextQueue;
    std::atomic<size_t> contention;

    void worker(size_t index);
  };
}

#endif
