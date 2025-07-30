#include "InputComponent.h"
#include "libraries/constants/Constants.h"
#include "libraries/keys/Keys.h"
#include "services/styling/StyleManager.h"
#include "utilities/color/ColorUtils.h"
#include "handlers/input/MouseHandler.h"
#include "handlers/input/CursorHandler.h"

namespace Project::Components {
  using Project::Libraries::Constants::DEFAULT_COMPONENT_SIZE;
  using Project::Libraries::Constants::DEFAULT_SHAPE_COLOR_HEX;
  using Project::Utilities::ColorUtils;
  using Project::Services::StyleManager;
  
  namespace Keys = Project::Libraries::Keys;
}
