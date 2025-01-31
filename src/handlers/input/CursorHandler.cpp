#include "CursorHandler.h"

namespace Project::Handlers {
  CursorHandler::CursorHandler(LogsManager& logsManager) : logsManager(logsManager) {
    defaultCursor = SDL_GetDefaultCursor();
    cursors[CursorState::DEFAULT] = defaultCursor;
  }

  CursorHandler::~CursorHandler() {
    cleanup();
  }

  void CursorHandler::loadCursor(CursorState state, const std::string& filePath, int hotspotX, int hotspotY) {
    SDL_Surface* surface = IMG_Load(filePath.c_str());

    if (logsManager.checkAndLogError(!surface, "Failed to load cursor image: " + filePath + " - " + std::string(SDL_GetError()))) {
      logsManager.flushLogs();
      return;
    }

    SDL_Cursor* cursor = SDL_CreateColorCursor(surface, hotspotX, hotspotY);
    if (cursor) {
      cursors[state] = cursor;
    } else {
      logsManager.logWarning("Failed to create SDL cursor.");
    }

    SDL_Texture* texture = nullptr;
    if (renderer) {

    }
    //fix this on weekends.

    cursorTextures[state] = texture;
  }

  void CursorHandler::setCursorState(CursorState state) {
    if (cursors.find(state) != cursors.end() && cursors[state]) {
      SDL_SetCursor(cursors[state]);
      currentState = state;
    } else {
      logsManager.logWarning("Cursor state not found: " + std::to_string(static_cast<int>(state)));
    }
  }

  void CursorHandler::setRenderer(SDL_Renderer* renderer) {
    this->renderer = renderer;
  }

  CursorState CursorHandler::getCursorState() const {
    return currentState;
  }

  SDL_Texture* CursorHandler::getCursorTexture(CursorState state) const {
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