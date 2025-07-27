#ifndef TRIGGER_COMPONENT_H
#define TRIGGER_COMPONENT_H

#include "components/BaseComponent.h"

#include <string>
#include <vector>

namespace Project { namespace Entities { class Entity; } }

namespace Project::Components {
  class TriggerComponent : public BaseComponent {
  private:

  
  public:
    Project::Entities::Entity* owner = nullptr;

    std::string condition;
    std::string callback;
    std::string keyName;
    std::string variable;

    void executeCallback(Project::Entities::Entity* target);
  };
}

#endif
