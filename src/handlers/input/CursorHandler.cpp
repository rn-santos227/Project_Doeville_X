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
  }
}