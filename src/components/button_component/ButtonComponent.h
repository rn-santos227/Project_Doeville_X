#ifndef BUTTON_COMPONENT_H
#define BUTTON_COMPONENT_H

#include <string>

#include <SDL.h>

#include "components/BaseComponent.h"
#include "components/PositionableComponent.h"
#include "interfaces/style_interface/Stylable.h"

namespace Project { namespace Entities { class Entity; } }
namespace Project { namespace Handlers { class MouseHandler; class CursorHandler; } }

namespace Project::Components {
  class ButtonComponent : public BaseComponent, public PositionableComponent, public Project::Interfaces::Stylable {
  public:

  
  private:
    Project::Handlers::CursorHandler* cursorHandler = nullptr;
    Project::Handlers::MouseHandler* mouseHandler = nullptr;

    
  };
}

#endif
