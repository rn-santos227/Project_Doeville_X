#ifndef CURSOR_HANDLER_H
#define CURSOR_HANDLER_H

#include "CursorState.h"

#include <string>
#include <unordered_map>

#include <SDL.h>

namespace Project::Handlers {
  class CursorHandler {
  public:
    CursorHandler(const std::string& resourcePath);
    ~CursorHandler();

    void loadCursor(CursorState state, const std::string& imagePath, int hotspotX = 0, int hotspotY = 0);

    void setCursorState(CursorState state);
    CursorState getCursorState() const;

    void resetToDefaultCursor();
    void cleanup();

  private:
    std::unordered_map<CursorState, SDL_Cursor*> cursors;
    SDL_Cursor* defaultCursor = nullptr;
    std::string resourcePath;
  };
}

#endif