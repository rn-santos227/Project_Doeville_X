#ifndef MOTION_COMPONENT_H
#define MOTION_COMPONENT_H

#include "components/BaseComponent.h"
#include "components/PositionableComponent.h"
#include "components/keys_component/KeysComponent.h"
#include "handlers/input/KeyHandler.h"
#include "libraries/constants/Constants.h"

namespace Project { namespace Entities { class Entity; } }

namespace Project::Components {
  class MotionComponent : public BaseComponent {
  public:
    MotionComponent(Project::Utilities::LogsManager& logsManager, Project::Handlers::KeyHandler* keyHandler, float speed = Project::Libraries::Constants::DEFAULT_MOTION_SPEED);
    ~MotionComponent() override = default;

    void update(float deltaTime) override;
    void render() override {}

    void setEntityReference(Project::Entities::Entity* entity) { owner = entity; }
    void setSpeed(float newSpeed) { speed = newSpeed; }
    float getSpeed() const { return speed; }

  private:
    Project::Handlers::KeyHandler* keyHandler = nullptr;
    Project::Entities::Entity* owner = nullptr;
    float speed;
  };
}

#endif
