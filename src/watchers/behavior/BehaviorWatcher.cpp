#include "BehaviorWatcher.h"

#include <utility>

namespace Project::Watchers {
  BehaviorWatcher::BehaviorWatcher(Condition condition, Command command)
  : condition(std::move(condition)), command(std::move(command)) {}
}
