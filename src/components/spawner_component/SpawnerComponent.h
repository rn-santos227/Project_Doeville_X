#ifndef SPAWNER_COMPONENT_H
#define SPAWNER_COMPONENT_H

#include "components/BaseComponent.h"

#include <lua.hpp>
#include <string>

namespace Project { namespace Entities { class Entity; } }

namespace Project::Components {
  class SpawnerComponent : public BaseComponent {
  public:


  private:
    Project::Entities::Entity* owner = nullptr;
    std::string templateName;
  };
}

#endif
