#ifndef MOUSE_HANDLER_H
#define MOUSE_HANDLER_H

#include <unordered_map>

#include <SDL.h>

#include "utilities/logs_manager/LogsManager.h"

namespace Project::Handlers {
  class MouseHandler {
  public:
    MouseHandler(Project::Utilities::LogsManager& logsManager);
    ~MouseHandler();

    int getMouseX() const;
    int getMouseY() const;
    int getMouseWheelScroll() const;

    bool isButtonDown(Uint8 button) const;
    bool isButtonUp(Uint8 button) const;

    void handleEvent(const SDL_Event& event);
    void updateMousePosition();

  private:
    Project::Utilities::LogsManager& logsManager;

    std::unordered_map<Uint8, bool> buttonStates;
    int mouseX;
    int mouseY;
    int mouseWheelScroll;
  };
}

#endif
