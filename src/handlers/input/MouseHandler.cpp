#include "MouseHandler.h"

MouseHandler::MouseHandler(LogsManager& logsManager) : mouseX(0), mouseY(0), mouseWheelScroll(0), logsManager(logsManager) {}
MouseHandler::~MouseHandler() {}

void MouseHandler::handleEvent(const SDL_Event& event) {
  switch (event.type) { 
    case SDL_MOUSEBUTTONDOWN:
      buttonStates[event.button.button] = true;
      break;

    case SDL_MOUSEBUTTONUP:
      buttonStates[event.button.button] = false;
      break;

    case SDL_MOUSEMOTION:
      mouseX = event.motion.x;
      mouseY = event.motion.y;
      break;

    case SDL_MOUSEWHEEL:
      mouseWheelScroll = event.wheel.y;
      break;

    default:
      break;
  }
}

bool MouseHandler::isButtonDown(Uint8 button) const {
  auto it = buttonStates.find(button);
  return it != buttonStates.end() && it->second;
}

bool MouseHandler::isButtonUp(Uint8 button) const {
  auto it = buttonStates.find(button);
  return it == buttonStates.end() || !it->second;
}