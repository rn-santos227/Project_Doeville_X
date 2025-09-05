#ifndef NETWORK_WATCHER_H
#define NETWORK_WATCHER_H

#include <atomic>
#include <thread>

#include "services/network/NetworkService.h"
#include "utilities/logs_manager/LogsManager.h"

namespace Project::Watchers {
  class NetworkWatcher {
  public:

  private:
    void start();
    void stop();
    void run();

    Project::Services::NetworkService& service;
    Project::Utilities::LogsManager& logsManager;

    std::atomic<bool> running{false};
    std::atomic<int> subscriberCount{0};
    std::thread worker;
  };
}

#endif
