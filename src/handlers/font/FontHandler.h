#ifndef FONT_HANDLER_H
#define FONT_HANDLER_H

#include <iostream>
#include <string>
#include <unordered_map>

#include <SDL_ttf.h>
#include <SDL.h>

#include "utilities/logs_manager/LogsManager.h"

namespace Project::Handlers {
  class FontHandler {
  public:
    FontHandler(Project::Utilities::LogsManager& logsManager);
    ~FontHandler();

    bool loadFont(const std::string& fontId, const std::string& path, int fontSize);
    SDL_Texture* renderText(SDL_Renderer* renderer, const std::string& text, const std::string& fontId, SDL_Color color);
    void cleanup();

  private:
    Project::Utilities::LogsManager& logsManager;

    std::unordered_map<std::string, TTF_Font*> fonts;
  };
}
#endif
