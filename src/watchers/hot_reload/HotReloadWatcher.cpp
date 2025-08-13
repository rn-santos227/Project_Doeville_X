#include "HotReloadWatcher.h"

#include <chrono>

namespace Project::Watchers {
  HotReloadWatcher::HotReloadWatcher(Project::Utilities::LogsManager& logs)
  : logsManager(logs) {}

  HotReloadWatcher::~HotReloadWatcher() {
    stop();
  }

  void HotReloadWatcher::watchFile(const std::string& path, std::function<void()> onChange) {

  }

  void HotReloadWatcher::stop() {

  }

  void HotReloadWatcher::threadFunc() {

  }
}
