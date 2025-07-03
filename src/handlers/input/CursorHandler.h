#ifndef CURSOR_HANDLER_H
#define CURSOR_HANDLER_H

#include "CursorState.h"

#include <string>
#include <unordered_map>

#include <SDL.h>
#include <SDL_image.h>

#include "utilities/logs_manager/LogsManager.h"
namespace Project::Handlers {
  class CursorHandler {
  public:
    CursorHandler(Project::Utilities::LogsManager& logsManager);
    ~CursorHandler();

    CursorState getCursorState() const;
    SDL_Texture* getCursorTexture(CursorState state) const;

    void loadCursor(CursorState state, const std::string& imagePath, int hotspotX = 0, int hotspotY = 0);
    
    void setCursorState(CursorState state);
    void setRenderer(SDL_Renderer* renderer);

    void resetToDefaultCursor();
    void cleanup();

  private:
    std::unordered_map<CursorState, SDL_Cursor*> cursors;
    std::unordered_map<CursorState, SDL_Texture*> cursorTextures;
    std::unordered_map<std::string, SDL_Cursor*> cursorCache;
    std::unordered_map<std::string, SDL_Texture*> textureCache;
    
    CursorState currentState = CursorState::DEFAULT;
    Project::Utilities::LogsManager& logsManager;

    SDL_Cursor* defaultCursor = nullptr;
    SDL_Renderer* renderer;
    SDL_Texture* currentCursorTexture = nullptr;
    
    std::string resourcePath;
  };
}
#endif
