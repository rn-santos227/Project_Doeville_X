#ifndef INPUT_COMPONENT_H
#define INPUT_COMPONENT_H

#include <array>
#include <string>

#include <SDL.h>
#include <SDL_ttf.h>

#include "components/BaseComponent.h"
#include "components/PositionableComponent.h"
#include "components/TextureHolder.h"
#include "interfaces/style_interface/Stylable.h"
#include "handlers/input/MouseHandler.h"
#include "handlers/input/CursorHandler.h"
#include "utilities/logs_manager/LogsManager.h"
#include "utilities/config_reader/ConfigReader.h"

namespace Project::Components {
  class InputComponent : 
    public BaseComponent, 
    public PositionableComponent,
    public TextureHolder, 
    public Project::Interfaces::Stylable {

  };
}

#endif
