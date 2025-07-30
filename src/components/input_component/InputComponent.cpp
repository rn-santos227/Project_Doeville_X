#include "InputComponent.h"

#include "handlers/input/MouseHandler.h"
#include "handlers/input/CursorHandler.h"
#include "libraries/constants/Constants.h"
#include "libraries/keys/Keys.h"
#include "services/styling/StyleManager.h"
#include "utilities/color/ColorUtils.h"

namespace Project::Components {
  using Project::Utilities::LogsManager;
  using Project::Utilities::ConfigReader;
  using Project::Handlers::CursorHandler;
  using Project::Handlers::MouseHandler;
  using Project::Utilities::ColorUtils;
  using Project::Services::StyleManager;

  namespace Constants = Project::Libraries::Constants;
  namespace Keys = Project::Libraries::Keys;

  InputComponent::InputComponent(SDL_Renderer* renderer, LogsManager& logsManager, ConfigReader& configReader,
   MouseHandler* mouseHandler, CursorHandler* cursorHandler)
    : BaseComponent(logsManager), renderer(renderer), configReader(configReader),
      mouseHandler(mouseHandler), cursorHandler(cursorHandler), font(nullptr) {}
}
