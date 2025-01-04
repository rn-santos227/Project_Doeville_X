#ifndef CURSOR_HANDLER_H
#define CURSOR_HANDLER_H

#include "CursorState.h"

#include <string>
#include <unordered_map>

#include <SDL.h>

#include "utilities/logs_manager/LogsManager.h"

using namespace Project::Utilities;

namespace Project::Handlers {
  class CursorHandler {
  public:
    CursorHandler(LogsManager& logsManager, const std::string& resourcePath);
    ~CursorHandler();

    CursorState getCursorState() const;
    SDL_Texture* getCursorTexture() const;

    void loadCursor(CursorState state, const std::string& imagePath, int hotspotX = 0, int hotspotY = 0);
    void setCursorState(CursorState state);

    void resetToDefaultCursor();
    void cleanup();

  private:
    std::unordered_map<CursorState, SDL_Cursor*> cursors;
    
    CursorState currentState = CursorState::DEFAULT;
    LogsManager& logsManager;
    SDL_Cursor* defaultCursor = nullptr;
    
    std::string resourcePath;
  };
}

#endif