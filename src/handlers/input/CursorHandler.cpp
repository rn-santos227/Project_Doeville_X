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

    SDL_Surface* surface = SDL_LoadBMP(fullPath.c_str());
    if (logsManager.checkAndLogError(!surface, "Failed to load cursor image: " + fullPath + " - " + std::string(SDL_GetError()))) {
      logsManager.flushLogs();
      return;
    }

    SDL_Cursor* cursor = SDL_CreateColorCursor(surface, hotspotX, hotspotY);
    SDL_FreeSurface(surface);

    if(logsManager.checkAndLogError(!cursor, "Failed to create cursor for state: " + std::string(SDL_GetError()))) {
      logsManager.flushLogs();
      return;
    }

    cursors[state] = cursor;
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

  }
}