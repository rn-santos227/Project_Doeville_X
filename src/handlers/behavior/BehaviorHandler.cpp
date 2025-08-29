#include "BehaviorHandler.h"

namespace Project::Handlers {
  using Project::Utilities::LogsManager;
  using Project::Watchers::BehaviorWatcher;

  BehaviorHandler::BehaviorHandler(LogsManager& logsManager)
  : logsManager(logsManager) {}

  void BehaviorHandler::update() {
    for (auto& watcher : watchers) {
      watcher.check();
    }
  }

  void BehaviorHandler::addWatcher(const BehaviorWatcher& watcher) {
    watchers.push_back(watcher);
  }
}
