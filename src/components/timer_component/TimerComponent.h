#ifndef TIMER_COMPONENT_H
#define TIMER_COMPONENT_H

#include "components/BaseComponent.h"

#include <string>

namespace Project { namespace Entities { class Entity; } }

namespace Project::Components {
  class TimerComponent : public BaseComponent {
  public:
    TimerComponent(Project::Utilities::LogsManager& logsManager, float seconds = 0.0f);
    ~TimerComponent() override = default;

    void update(float deltaTime) override;
    void render() override {}
    void build(Project::Utilities::LuaStateWrapper& luaStateWrapper, const std::string& tableName) override;

    void stop();
    void reset() { elapsed = 0.0f; }

    void setEntityReference(Project::Entities::Entity* entity) { owner = entity; }
    void setCallback(const std::string& functionName) { luaFunction = functionName; }
    void setDuration(float seconds) { duration = seconds; }
    void setRepeat(bool value) { repeat = value; }
    
    float getDuration() const { return duration; }
    bool isRepeating() const { return repeat; }

  private:
    Project::Entities::Entity* owner = nullptr;
    std::string luaFunction;
    
    float duration = 0.0f;
    float elapsed = 0.0f;

    bool repeat = false;
  };
}

#endif
