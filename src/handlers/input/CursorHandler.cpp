#include "CursorHandler.h"

namespace Project::Handlers {
  CursorHandler::CursorHandler(LogsManager& logsManager, const std::string& resourcePath) : logsManager(logsManager), resourcePath(resourcePath) {
    defaultCursor = SDL_GetDefaultCursor();
    cursors[CursorState::DEFAULT] = defaultCursor;
  }

  CursorHandler::~CursorHandler() {
    
  }
}