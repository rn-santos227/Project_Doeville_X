#include "HotReloadWatcher.h"

#include <chrono>

namespace Project::Watchers {
  HotReloadWatcher::HotReloadWatcher(Project::Utilities::LogsManager& logs)
  : logsManager(logs) {}

  HotReloadWatcher::~HotReloadWatcher() {
    stop();
  }

  void HotReloadWatcher::watchFile(const std::string& path, std::function<void()> onChange) {
    std::lock_guard<std::mutex> lock(watchMutex);
    std::filesystem::file_time_type ts;
    if (std::filesystem::exists(path)) {
      ts = std::filesystem::last_write_time(path);
    }
    watches.push_back({path, ts, onChange});
    if (!running) {
      running = true;
      worker = std::thread(&HotReloadWatcher::threadFunc, this);
    }
  }

  void HotReloadWatcher::stop() {
    if (running) {
      running = false;
      if (worker.joinable()) worker.join();
    }
  }

  void HotReloadWatcher::threadFunc() {
    using namespace std::chrono_literals;
    while (running) {
      {
        std::lock_guard<std::mutex> lock(watchMutex);
        for (auto& w : watches) {
          if (!std::filesystem::exists(w.path)) continue;
          auto current = std::filesystem::last_write_time(w.path);
          if (w.lastWrite != current) {
            w.lastWrite = current;
            logsManager.logMessage("Hot reloading " + w.path);
            if (w.callback) w.callback();
          }
        }
      }
      std::this_thread::sleep_for(1s);
    }
  }
}
