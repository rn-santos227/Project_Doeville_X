#ifndef HOT_RELOAD_WATCHER_H
#define HOT_RELOAD_WATCHER_H

#include <filesystem>
#include <functional>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include "utilities/logs_manager/LogsManager.h"

namespace Project::Watchers {
  class HotReloadWatcher {
  public:
    explicit HotReloadWatcher(Project::Utilities::LogsManager& logs);
    ~HotReloadWatcher();

    void watchFile(const std::string& path, std::function<void()> onChange);
    void stop();

  private:
    struct Watch {
      std::string path;
      std::filesystem::file_time_type lastWrite;
      std::function<void()> callback;
    };

    void threadFunc();

    Project::Utilities::LogsManager& logsManager;
    std::vector<Watch> watches;
    std::thread worker;
    std::mutex watchMutex;
    bool running{false};
  };
}

#endif
