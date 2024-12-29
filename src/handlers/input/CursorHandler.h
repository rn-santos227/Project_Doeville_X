#ifndef CURSOR_HANDLER_H
#define CURSOR_HANDLER_H

#include <string>
#include <unordered_map>

#include <SDL.h>

namespace Project::Handlers {
  class CursorHandler {
    public:
      CursorHandler(const std::string& resourcePath);
      ~CursorHandler();

    private:
      std::string resourcePath;
  };
}

#endif