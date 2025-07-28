#include "FontHandler.h"

#include "helpers/resource_cleaner/ResourceCleaner.h"
#include "libraries/constants/Constants.h"

namespace Project::Handlers {
  using Project::Utilities::LogsManager;
  using Project::Helpers::ResourceCleaner;

  namespace Constants = Project::Libraries::Constants;

  FontHandler::FontHandler(LogsManager& logsManager) : logsManager(logsManager) {
    logsManager.checkAndLogError(TTF_Init() == -1, "Failed to initialize SDL_ttf: " + std::string(TTF_GetError()));
  }

  FontHandler::~FontHandler() {
    cleanup();
    TTF_Quit();
  }

  bool FontHandler::loadFont(const std::string& fontId, const std::string& filePath, int fontSize) {
    TTF_Font* font = TTF_OpenFont(filePath.c_str(), fontSize);
    if (!font) {
      logsManager.logWarning(std::string("Failed to load font: ") + filePath + ". Using fallback font.");
      font = TTF_OpenFont(Constants::DEFAULT_FONT_PATH, fontSize);
      if (!font) {
        logsManager.logError(std::string("Failed to load fallback font: ") + Constants::DEFAULT_FONT_PATH);
        return false;
      }
    }

    fonts[fontId] = font;
    return true;
  }

  SDL_Texture* FontHandler::renderText(SDL_Renderer* renderer, const std::string& text, const std::string& fontId, SDL_Color color) {
    auto it = fonts.find(fontId);
    if (it == fonts.end()) {
      logsManager.logWarning("Font ID \"" + fontId + "\" not found. Using fallback font.");
      it = fonts.find(Constants::DEFAULT_FONT);
      if (it == fonts.end()) {
        logsManager.logError("Fallback font not loaded.");
        return getFallbackTexture(renderer);
      }
    }

    TTF_Font* font = it->second;
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, text.c_str(), color);

    if (!textSurface) {
      logsManager.logWarning(std::string("Failed to create text surface: ") + TTF_GetError());
      return getFallbackTexture(renderer);
    }


    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_FreeSurface(textSurface);

    if (!textTexture) {
      logsManager.logWarning(std::string("Failed to create texture from text: ") + SDL_GetError());
      return getFallbackTexture(renderer);
    }
    return textTexture;
  }

  void FontHandler::cleanup() {
    ResourceCleaner::cleanupMap(fonts, [](TTF_Font* font) {
      if (font) {
        TTF_CloseFont(font);
      }
    });
    fonts.clear();
    ResourceCleaner::cleanupMap(fallbackTextures, [](SDL_Texture* texture) {
      if (texture) {
        SDL_DestroyTexture(texture);
      }
    });
    fallbackTextures.clear();
  }

  SDL_Texture* FontHandler::getFallbackTexture(SDL_Renderer* renderer) {
    auto it = fallbackTextures.find(renderer);
    if (it != fallbackTextures.end()) {
      return it->second;
    }

    SDL_Surface* surface = SDL_CreateRGBSurfaceWithFormat(0, Constants::INDEX_TWO, Constants::INDEX_TWO, Constants::BIT_32, SDL_PIXELFORMAT_RGBA32);
    if (!surface) {
      logsManager.logError(std::string("Failed to create fallback surface: ") + SDL_GetError());
      return nullptr;
    }
    
    Uint32 magenta = SDL_MapRGBA(surface->format, Constants::BIT_255, 0, Constants::BIT_255, Constants::BIT_255);
    Uint32 black = SDL_MapRGBA(surface->format, 0, 0, 0, Constants::BIT_255);
    Uint32* pixels = static_cast<Uint32*>(surface->pixels);
    pixels[Constants::INDEX_ZERO] = magenta;
    pixels[Constants::INDEX_ONE] = black;
    pixels[Constants::INDEX_TWO] = black;
    pixels[Constants::INDEX_THREE] = magenta;

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    if (!texture) {
      logsManager.logError(std::string("Failed to create fallback texture: ") + SDL_GetError());
      return nullptr;
    }

    fallbackTextures[renderer] = texture;
    return texture;
  }
}
