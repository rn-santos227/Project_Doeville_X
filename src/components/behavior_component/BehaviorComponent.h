#ifndef BEHAVIOR_COMPONENT_H
#define BEHAVIOR_COMPONENT_H

#include <string>

#include "components/BaseComponent.h"
#include "handlers/behavior/BehaviorHandler.h"

namespace Project { namespace Entities { class Entity; } }

namespace Project::Components {
  class BehaviorComponent : public BaseComponent {
  public:
    explicit BehaviorComponent(Project::Utilities::LogsManager& logsManager);
    ~BehaviorComponent() override = default;

  private:
    Project::Entities::Entity* owner = nullptr;
    Project::Handlers::BehaviorHandler handler;
    std::string scriptPath;
  };
}

#endif
