#ifndef TIMER_COMPONENT_H
#define TIMER_COMPONENT_H

#include "TimerData.h"


#include <string>

#include "components/BaseComponent.h"


namespace Project { namespace Entities { class Entity; } }

namespace Project::Components {
  class TimerComponent : public BaseComponent {
  public:
    TimerComponent(Project::Utilities::LogsManager& logsManager, float seconds = 0.0f);
    ~TimerComponent() override = default;

    Project::Entities::Entity* getOwner() const override { return owner; }
    ComponentType getType() const override { return ComponentType::TIMER; }

    void update(float deltaTime) override;
    void render() override {}
    void build(Project::Utilities::LuaStateWrapper& luaStateWrapper, const std::string& tableName) override;

    void stop();
    void reset() { data.elapsed = 0.0f; }

    void setEntityReference(Project::Entities::Entity* entity) { owner = entity; }
    void setCallback(const std::string& functionName) { data.luaCallback  = functionName; }
    void setDuration(float seconds) { data.duration = seconds; }
    void setRepeat(bool value) { data.repeat = value; }
    
    float getDuration() const { return data.duration; }
    bool isRepeating() const { return data.repeat; }

  private:
    Project::Entities::Entity* owner = nullptr;
    TimerData data;
  };
}

#endif
