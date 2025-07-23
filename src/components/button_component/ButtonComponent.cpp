#include "ButtonComponent.h"

#include "entities/Entity.h"

#include "libraries/constants/Constants.h"
#include "libraries/keys/Keys.h"
#include "utilities/color/ColorUtils.h"

namespace Project::Components {
  using Project::Utilities::ColorUtils;
  using Project::Handlers::CursorHandler;
  using Project::Handlers::MouseHandler;

  namespace Constants = Project::Libraries::Constants;
  namespace Keys = Project::Libraries::Keys;
  
  ButtonComponent::ButtonComponent(SDL_Renderer* renderer, Project::Utilities::LogsManager& logsManager,
    Project::Handlers::MouseHandler* mouseHandler, Project::Handlers::CursorHandler* cursorHandler)
      : BaseComponent(logsManager), mouseHandler(mouseHandler), cursorHandler(cursorHandler), renderer(renderer) {}

   void ButtonComponent::update(float /*deltaTime*/) {
    if (!mouseHandler || !cursorHandler) return;

    int x = mouseHandler->getMouseX();
    int y = mouseHandler->getMouseY();
    
    SDL_Point mousePoint = {x, y};
    bool inside = SDL_PointInRect(&mousePoint, &rect);

    if (inside) {
      if (!hovered) {
        hovered = true;
        cursorHandler->setCursorState(Project::Handlers::CursorState::HOVER);
      }

      bool pressed = mouseHandler->isButtonDown(SDL_BUTTON_LEFT);
      if (pressed && !wasPressed) {
        if (owner && !luaFunction.empty()) {
          owner->callLuaFunction(luaFunction);
        }
      }
      wasPressed = pressed;
    } else {
      if (hovered) {
        hovered = false;
        cursorHandler->setCursorState(Project::Handlers::CursorState::DEFAULT);
      }
      wasPressed = false;
    }
  }

  void ButtonComponent::render() {
    if (!renderer) return;
    SDL_Color drawColor = hovered ? hoverColor : color;
    SDL_SetRenderDrawColor(renderer, drawColor.r, drawColor.g, drawColor.b, drawColor.a);
    SDL_RenderFillRect(renderer, &rect);
  }
}
