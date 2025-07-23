#include "ButtonComponent.h"

#include "entities/Entity.h"
#include "libraries/constants/Constants.h"
#include "libraries/keys/Keys.h"
#include "utilities/color/ColorUtils.h"

namespace Project::Components {
  using Project::Utilities::ColorUtils;

  namespace Constants = Project::Libraries::Constants;
  namespace Keys = Project::Libraries::Keys;
  
  ButtonComponent::ButtonComponent(SDL_Renderer* renderer, Project::Utilities::LogsManager& logsManager,
    Project::Handlers::MouseHandler* mouseHandler, Project::Handlers::CursorHandler* cursorHandler)
      : BaseComponent(logsManager), mouseHandler(mouseHandler), cursorHandler(cursorHandler), renderer(renderer) {}

   void ButtonComponent::update(float /*deltaTime*/) {
    if (!mouseHandler || !cursorHandler) return;
   }
}
