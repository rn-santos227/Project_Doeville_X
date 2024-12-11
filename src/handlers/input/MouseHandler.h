#ifndef MOUSE_HANDLER_H
#define MOUSE_HANDLER_H

#include <unordered_map>

#include <SDL2/SDL.h>

#include "utilities/logs_manager/LogsManager.h"

class MouseHandler {
public:
  MouseHandler(LogsManager& logsManager);
  ~MouseHandler();

  void handleEvent(const SDL_Event& event);
  bool isButtonDown(Uint8 button) const;
  bool isButtonUp(Uint8 button) const;
  int getMouseX() const;
  int getMouseY() const;
  int getMouseWheelScroll() const;

private:
  LogsManager& logsManager;
  
  std::unordered_map<Uint8, bool> buttonStates;
  int mouseX;
  int mouseY;
  int mouseWheelScroll;
};

#endif