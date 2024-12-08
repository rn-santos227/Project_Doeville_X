#ifndef FONT_HANDLER_H
#define FONT_HANDLER_H

#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL.h>
#include <string>
#include <unordered_map>

class FontHandler {
public:
  FontHandler();
  ~FontHandler();

  bool loadFont(const std::string& fontId, const std::string& filePath, int fontSize);
  SDL_Texture* renderText(SDL_Renderer* renderer, const std::string& text, const std::string& fontId, SDL_Color color);
  void cleanup();

private:
  std::unordered_map<std::string, TTF_Font*> fonts;
};

#endif