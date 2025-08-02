#include "TimerComponent.h"

#include "entities/Entity.h"
#include "libraries/constants/Constants.h"
#include "libraries/keys/Keys.h"

namespace Project::Components {
  namespace Keys = Project::Libraries::Keys;

  TimerComponent::TimerComponent(Project::Utilities::LogsManager& logsManager, float seconds)
    : BaseComponent(logsManager) {
    data.duration = seconds;
  }

  void TimerComponent::update(float deltaTime) {
    if (!isActive() || !owner || data.luaCallback.empty() || data.duration <= 0.0f) return;

    data.elapsed += deltaTime;
    if (data.elapsed >= data.duration) {
      owner->callLuaFunction(data.luaCallback);
      if (data.repeat) {
        data.elapsed = 0.0f;
      } else {
        stop();
      }
    }
  }

  void TimerComponent::build(Project::Utilities::LuaStateWrapper& luaStateWrapper, const std::string& tableName) {
    data.duration = static_cast<float>(luaStateWrapper.getTableNumber(tableName, Keys::SECONDS, data.duration));
    if (data.duration <= 0.0f) {
      data.duration = static_cast<float>(luaStateWrapper.getTableNumber(tableName, Keys::TIME, data.duration));
    }
    
    data.luaCallback = luaStateWrapper.getTableString(tableName, Keys::CALLBACKS, Project::Libraries::Constants::EMPTY_STRING);
    data.repeat = luaStateWrapper.getTableBoolean(tableName, Keys::REPEAT, false);
  }

  void TimerComponent::stop() {
    setActive(false);
    data.reset();
  }
}
