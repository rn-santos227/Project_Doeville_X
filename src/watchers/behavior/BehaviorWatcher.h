#ifndef BEHAVIOR_WATCHER_H
#define BEHAVIOR_WATCHER_H

#include <functional>

namespace Project::Watchers {
  class BehaviorWatcher {
  public:
    using Condition = std::function<bool()>;
    using Command = std::function<void()>;

    BehaviorWatcher(Condition condition, Command command);
    void check();

  private:
    Condition condition;
    Command command;
  };
}

#endif
