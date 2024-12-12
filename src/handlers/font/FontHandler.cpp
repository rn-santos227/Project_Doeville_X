#include "FontHandler.h"
namespace Project::Handlers {
  FontHandler::FontHandler(LogsManager& logsManager) : logsManager() {
    logsManager.checkAndLogError(TTF_Init() == -1, "Failed to initialize SDL_ttf: " + std::string(TTF_GetError()));
  }

  FontHandler::~FontHandler() {
    cleanup();
    TTF_Quit();
  }

  bool FontHandler::loadFont(const std::string& fontId, const std::string& filePath, int fontSize) {
    TTF_Font* font = TTF_OpenFont(filePath.c_str(), fontSize);
    if(logsManager.checkAndLogError(!font,  "Failed to initialize SDL_ttf: "  + std::string(TTF_GetError()))) {
      logsManager.flushLogs();
      return false;
    }

    fonts[fontId] = font;
    return true;
  }

  SDL_Texture* FontHandler::renderText(SDL_Renderer* renderer, const std::string& text, const std::string& fontId, SDL_Color color) {
    auto it = fonts.find(fontId);
    if (logsManager.checkAndLogError(it == fonts.end(), "Font ID \"" + fontId + "\" not found!")) {
      logsManager.flushLogs();
      return nullptr;
    }

    TTF_Font* font = it->second;
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, text.c_str(), color);

    if (logsManager.checkAndLogError(!textSurface, "Failed to create text surface: " + std::string(TTF_GetError()))) {
      logsManager.flushLogs();
      return nullptr;
    }

    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_FreeSurface(textSurface);

    logsManager.checkAndLogError(!textTexture, "Failed to create texture from text: " + std::string(SDL_GetError()));
    return textTexture;
  }

  void FontHandler::cleanup() {
    for (auto& pair : fonts) {
      TTF_CloseFont(pair.second);
    }
    fonts.clear();
  }
}