#include "BehaviorComponent.h"

#include "libraries/constants/Constants.h"
#include "libraries/keys/Keys.h"

namespace Project::Components {
  BehaviorComponent::BehaviorComponent(Project::Utilities::LogsManager& logsManager)
    : BaseComponent(logsManager), handler(logsManager) {}

  void BehaviorComponent::update(float /*deltaTime*/) {
    handler.update();
  }

  void BehaviorComponent::build(Project::Utilities::LuaStateWrapper& luaStateWrapper, const std::string& tableName) {
    scriptPath = luaStateWrapper.getTableString(tableName, Project::Libraries::Keys::SCRIPT_PATH, Project::Libraries::Constants::EMPTY_STRING);
    if (!scriptPath.empty()) {
      luaStateWrapper.loadScript(scriptPath);
    }
  }
}
