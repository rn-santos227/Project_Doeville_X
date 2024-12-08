#include "FontHandler.h"
#include <iostream>

FontHandler::FontHandler() {
  if (TTF_Init() == -1) {
    std::cerr << "Failed to initialize SDL_ttf: " << TTF_GetError() << std::endl;
  }
}

FontHandler::~FontHandler() {
  cleanup();
  TTF_Quit();
}

bool FontHandler::loadFont(const std::string& fontId, const std::string& filePath, int fontSize) {
  TTF_Font* font = TTF_OpenFont(filePath.c_str(), fontSize);
  if(!font) {
    std::cerr << "Failed to load font (" << filePath << "): " << TTF_GetError() << std::endl;
    return false;
  }

  fonts[fontId] = font;
  return true;
}

SDL_Texture* FontHandler::renderText(SDL_Renderer* renderer, const std::string& text, const std::string& fontId, SDL_Color color) {
  auto it = fonts.find(fontId);
  if (it == fonts.end()) {
    std::cerr << "Font ID \"" << fontId << "\" not found!" << std::endl;
    return nullptr;
  }

  TTF_Font* font = it->second;
  SDL_Surface* textSurface = TTF_RenderText_Solid(font, text.c_str(), color);
  if (!textSurface) {
    std::cerr << "Failed to create text surface: " << TTF_GetError() << std::endl;
    return nullptr;
  }

  SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
  SDL_FreeSurface(textSurface);
  if (!textTexture) {
    std::cerr << "Failed to create texture from text: " << SDL_GetError() << std::endl;
  }

  return textTexture;
}

void FontHandler::cleanup() {
  for (auto& pair : fonts) {
    TTF_CloseFont(pair.second);
  }
  fonts.clear();
}