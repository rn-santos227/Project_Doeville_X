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

    private:
      std::string resourcePath;
  };
}

#endif