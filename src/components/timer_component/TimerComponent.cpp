#include "TimerComponent.h"

#include "entities/Entity.h"
#include "libraries/constants/TimeConstants.h"
#include "libraries/keys/Keys.h"

namespace Project::Components {
  namespace Keys = Project::Libraries::Keys;

  TimerComponent::TimerComponent(Project::Utilities::LogsManager& logsManager, float seconds)
    : BaseComponent(logsManager), duration(seconds) {}

  void TimerComponent::update(float deltaTime) {
    if (!isActive() || !owner || luaFunction.empty() || duration <= 0.0f) return;

    elapsed += deltaTime;
    if (elapsed >= duration) {
      owner->callLuaFunction(luaFunction);
      if (repeat) {
        elapsed = 0.0f;
      } else {
        stop();
      }
    }
  }

  void TimerComponent::build(Project::Utilities::LuaStateWrapper& luaStateWrapper, const std::string& tableName) {
    duration = static_cast<float>(luaStateWrapper.getTableNumber(tableName, Keys::SECONDS, duration));
    if (duration <= 0.0f) {
      duration = static_cast<float>(luaStateWrapper.getTableNumber(tableName, Keys::TIME, duration));
    }
    
    luaFunction = luaStateWrapper.getTableString(tableName, Keys::FUNCTION, Constants::EMPTY_STRING);
    repeat = luaStateWrapper.getTableBoolean(tableName, Keys::REPEAT, false);
  }

  void TimerComponent::stop() {
    active = false;
    reset();
  }
}
