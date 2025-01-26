#ifndef CURSOR_HANDLER_H
#define CURSOR_HANDLER_H

#include "CursorState.h"

#include <string>
#include <unordered_map>

#include <SDL.h>
#include <SDL_image.h>

#include "utilities/logs_manager/LogsManager.h"

using namespace Project::Utilities;

namespace Project::Handlers {
  class CursorHandler {
  public:
    CursorHandler(LogsManager& logsManager);
    ~CursorHandler();

    CursorState getCursorState() const;
    SDL_Texture* getCursorTexture(CursorState state) const;

    void loadCursor(CursorState state, const std::string& imagePath, int hotspotX = 0, int hotspotY = 0);
    void setCursorState(CursorState state);

    void resetToDefaultCursor();
    void cleanup();

  private:
    std::unordered_map<CursorState, SDL_Cursor*> cursors;
    std::unordered_map<CursorState, SDL_Texture*> cursorTextures;
    
    CursorState currentState = CursorState::DEFAULT;
    LogsManager& logsManager;
    
    SDL_Cursor* defaultCursor = nullptr;
    SDL_Renderer* renderer;
    SDL_Texture* currentCursorTexture = nullptr;
    
    std::string resourcePath;
  };
}

#endif