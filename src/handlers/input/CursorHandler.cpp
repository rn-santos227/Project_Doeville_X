#include "CursorHandler.h"

namespace Project::Handlers {
  CursorHandler::CursorHandler(LogsManager& logsManager) : logsManager(logsManager) {
    defaultCursor = SDL_GetDefaultCursor();
    cursors[CursorState::DEFAULT] = defaultCursor;
  }

  CursorHandler::~CursorHandler() {
    cleanup();
  }

  void CursorHandler::loadCursor(CursorState state, const std::string& cursorPath, int hotspotX, int hotspotY) {
    SDL_Surface* surface = SDL_LoadBMP(cursorPath.c_str());

    if (logsManager.checkAndLogError(!surface, "Failed to load cursor image: " + fullPath + " - " + std::string(SDL_GetError()))) {

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

  SDL_Texture* CursorHandler::getCursorTexture(CursorState state) {
    auto it = cursorTextures.find(state);
    
    if (it != cursorTextures.end()) {
      return it->second;
    } else {
      logsManager.logWarning("Current cursor texture is null.");
      return nullptr;
    }
  }

  void CursorHandler::resetToDefaultCursor() {
    auto it = cursorTextures.find(CursorState::DEFAULT);

    if (it != cursorTextures.end()) {
      currentCursorTexture = it->second;
    } else {
      logsManager.logError("Default cursor texture not found.");
    }

    currentState = CursorState::DEFAULT;
    SDL_SetCursor(cursors[CursorState::DEFAULT]);
  }

  void CursorHandler::cleanup() {
    for (auto& pair : cursors) {
      if (pair.second) {
        SDL_FreeCursor(pair.second);
      }
    }
    cursors.clear();

    for (auto& pair : cursorTextures) {
      if (pair.second) {
        SDL_DestroyTexture(pair.second);
      }
    }
    cursorTextures.clear();
    currentCursorTexture = nullptr;
  }
}