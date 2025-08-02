#ifndef MODAL_COMPONENT_H
#define MODAL_COMPONENT_H

#include "ModalType.h"

#include <string>
#include <SDL.h>
#include <SDL_ttf.h>

#include "components/BaseComponent.h"
#include "components/PositionableComponent.h"
#include "interfaces/style_interface/Stylable.h"
#include "handlers/input/MouseHandler.h"
#include "utilities/config_reader/ConfigReader.h"

namespace Project { namespace Entities { class Entity; } }

namespace Project::Components {
  class ModalComponent : public BaseComponent, public PositionableComponent, public Project::Interfaces::Stylable {
  public:
    ModalComponent(
      SDL_Renderer* renderer,
      Project::Utilities::LogsManager& logsManager,
      Project::Utilities::ConfigReader& configReader,
      Project::Handlers::MouseHandler* mouseHandler
    );
    ~ModalComponent() override;
  };
}

#endif
