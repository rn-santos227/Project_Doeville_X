#include "CursorHandler.h"

namespace Project::Handlers {
  CursorHandler::CursorHandler(LogsManager& logsManager, const std::string& resourcePath) : logsManager(logsManager), resourcePath(resourcePath) {
    defaultCursor = SDL_GetDefaultCursor();
    cursors[CursorState::DEFAULT] = defaultCursor;
  }

  CursorHandler::~CursorHandler() {
    cleanup();
  }

  void CursorHandler::loadCursor(CursorState state, const std::string& imagePath, int hotspotX, int hotspotY) {
    std::string fullPath = resourcePath + "/" + imagePath;

    SDL_Cursor* cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
    if (!cursor) {
      logsManager.logError("Failed to load system cursor: " + std::string(SDL_GetError()));
      return;
    }

    cursors[state] = cursor;
    SDL_Surface* surface = SDL_LoadBMP(imagePath.c_str());

    if (!surface) {
      logsManager.logError("Failed to load cursor surface: " + std::string(SDL_GetError()));
      return;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(SDL_GetRenderer(SDL_GetWindowFromID(1)), surface);
    SDL_FreeSurface(surface);

    if (!texture) {

    }
  }

  void CursorHandler::setCursorState(CursorState state) {
    if (cursors.find(state) != cursors.end() && cursors[state]) {
      SDL_SetCursor(cursors[state]);
      currentState = state;
    } else {
      logsManager.logWarning("Cursor state not found: " + std::to_string(static_cast<int>(state)));
    }
  }

  CursorState CursorHandler::getCursorState() const {
    return currentState;
  }

  void CursorHandler::resetToDefaultCursor() {
    if (defaultCursor) {
      SDL_SetCursor(defaultCursor);
      SDL_ShowCursor(SDL_ENABLE);
      currentState = CursorState::DEFAULT;
    } else {
      logsManager.logError("Default cursor is null.");
    }
  }

  void CursorHandler::cleanup() {
    for (auto& pair : cursors) {
      if (pair.second && pair.second != defaultCursor) {
        SDL_FreeCursor(pair.second);
      }
    }
    cursors.clear();
  }
}