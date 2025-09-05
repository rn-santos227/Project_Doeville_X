#include "NetworkWatcher.h"

#include <chrono>

namespace Project::Watchers {
  using Project::Utilities::LogsManager;
  using Project::Services::NetworkService;

  NetworkWatcher::NetworkWatcher(NetworkService& networkService, LogsManager& logsManager)
    : networkService(networkService), logsManager(logsManager) {}

  NetworkWatcher::~NetworkWatcher() { stop(); }

  void NetworkWatcher::start() {
    if (running) return;
    running = true;
    worker = std::thread(&NetworkWatcher::run, this);
  }

  void NetworkWatcher::stop() {
    running = false;
    if (worker.joinable()) worker.join();
  }

  void NetworkWatcher::run() {
    while (running) {
      networkService.poll();
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
  }
}
