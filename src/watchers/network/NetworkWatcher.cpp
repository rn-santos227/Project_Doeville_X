#include "NetworkWatcher.h"

#include <chrono>

namespace Project::Watchers {
  using Project::Utilities::LogsManager;
  using Project::Services::NetworkService;

  NetworkWatcher::NetworkWatcher(NetworkService& networkService, LogsManager& logsManager)
    : networkService(networkService), logsManager(logsManager) {}

  NetworkWatcher::~NetworkWatcher() { stop(); }
}
