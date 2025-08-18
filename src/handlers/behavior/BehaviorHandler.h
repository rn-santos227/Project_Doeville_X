#ifndef BEHAVIOR_HANDLER_H
#define BEHAVIOR_HANDLER_H

#include <string>
#include <vector>

#include "watchers/behavior/BehaviorWatcher.h"
#include "utilities/logs_manager/LogsManager.h"

namespace Project::Handlers {
  class BehaviorHandler {
  public:
    explicit BehaviorHandler(Project::Utilities::LogsManager& logsManager);

    void update();
    void addWatcher(const Project::Watchers::BehaviorWatcher& watcher);

    void setState(const std::string& newState) { state = newState; }
    const std::string& getState() const { return state; }

  private:
    Project::Utilities::LogsManager& logsManager;
    std::string state;
    std::vector<Project::Watchers::BehaviorWatcher> watchers;
  };
}

#endif
