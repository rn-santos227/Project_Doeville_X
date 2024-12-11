#ifndef FONT_HANDLER_H
#define FONT_HANDLER_H

#include <SDL_ttf.h>
#include <SDL.h>
#include <string>
#include <unordered_map>

#include "utilities/logs_manager/LogsManager.h"

class FontHandler {
public:
  FontHandler(LogsManager& LogsManager);
  ~FontHandler();

  bool loadFont(const std::string& fontId, const std::string& path, int fontSize);
  SDL_Texture* renderText(SDL_Renderer* renderer, const std::string& text, const std::string& fontId, SDL_Color color);
  void cleanup();

private:
  LogsManager logsManager;
  
  std::unordered_map<std::string, TTF_Font*> fonts;
};

#endif