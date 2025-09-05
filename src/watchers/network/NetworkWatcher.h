#ifndef NETWORK_WATCHER_H
#define NETWORK_WATCHER_H

#include <atomic>
#include <thread>

#include "services/network/NetworkService.h"
#include "utilities/logs_manager/LogsManager.h"

namespace Project::Watchers {
  class NetworkWatcher {
  public:
    NetworkWatcher(Project::Services::NetworkService& networkService, Project::Utilities::LogsManager& logsManager);
    ~NetworkWatcher();

    void subscribe();
    void unsubscribe();

  private:
    void start();
    void stop();
    void run();

    Project::Utilities::LogsManager& logsManager;
    Project::Services::NetworkService& networkService;

    std::atomic<bool> running{false};
    std::atomic<int> subscriberCount{0};
    std::thread worker;
  };
}

#endif
