#include "BehaviorHandler.h"

namespace Project::Handlers {
  BehaviorHandler::BehaviorHandler(Project::Utilities::LogsManager& logsManager)
  : logsManager(logsManager) {}

  void BehaviorHandler::update() {
    for (auto& watcher : watchers) {
      watcher.check();
    }
  }
}
