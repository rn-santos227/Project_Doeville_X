#include "CursorHandler.h"

#include "helpers/resource_cleaner/ResourceCleaner.h"
#include "libraries/constants/Constants.h"

namespace Project::Handlers {
  using Project::Utilities::LogsManager;
  using Project::Helpers::ResourceCleaner;

  namespace Constants = Project::Libraries::Constants;

  CursorHandler::CursorHandler(LogsManager& logsManager) 
    : logsManager(logsManager), renderer(nullptr), currentCursorTexture(nullptr), currentState(CursorState::DEFAULT) {
    defaultCursor = SDL_GetDefaultCursor();
    cursors[CursorState::DEFAULT] = defaultCursor;
  }

  CursorHandler::~CursorHandler() {
    cleanup();
  }

  void CursorHandler::loadCursor(CursorState state, const std::string& filePath, int hotspotX, int hotspotY) {
    logsManager.logMessage("Loading cursor from: " + filePath);
    SDL_Cursor* cursor = nullptr;
    SDL_Texture* texture = nullptr;

    auto cIt = cursorCache.find(filePath);
    auto tIt = textureCache.find(filePath);

    if (cIt != cursorCache.end() && tIt != textureCache.end()) {
      cursor = cIt->second;
      texture = tIt->second;
    } else {
      SDL_Surface* surface = IMG_Load(filePath.c_str());
      if (!surface) {
        logsManager.logWarning(std::string("Failed to load cursor image: ") + filePath + " - " + SDL_GetError() + ". Using fallback cursor.");
        cursor = defaultCursor;
        texture = getFallbackTexture();
      } else {
        logsManager.logMessage("Successfully loaded cursor image: " + filePath);
        cursor = SDL_CreateColorCursor(surface, hotspotX, hotspotY);
        if (cursor) {
          cursorCache[filePath] = cursor;
        } else {
          logsManager.logWarning("Failed to create SDL cursor. Using default cursor.");
          cursor = defaultCursor;
        }

        if (renderer) {
          texture = SDL_CreateTextureFromSurface(renderer, surface);
          if (!texture) {
            logsManager.logWarning("Failed to create texture for cursor: " + filePath + " - " + std::string(SDL_GetError()) + ". Using fallback texture.");
            texture = getFallbackTexture();
          } else {
            textureCache[filePath] = texture;
          }
        } else {
          logsManager.logError("Renderer is null. Cannot create texture.");
          texture = getFallbackTexture();
        }
        SDL_FreeSurface(surface);
      }
    }

    cursors[state] = cursor ? cursor : defaultCursor;
    cursorTextures[state] = texture ? texture : getFallbackTexture();
  }

  void CursorHandler::loadCursorSurface(CursorState state, SDL_Surface* surface, int hotspotX, int hotspotY) {
    if (!surface) {
      cursors[state] = defaultCursor;
      cursorTextures[state] = getFallbackTexture();
      return;
    }

    SDL_Cursor* cursor = SDL_CreateColorCursor(surface, hotspotX, hotspotY);
    if (!cursor) cursor = defaultCursor;

    SDL_Texture* texture = nullptr;
    if (renderer) {
      texture = SDL_CreateTextureFromSurface(renderer, surface);
      if (!texture) texture = getFallbackTexture();
    } else {
      logsManager.logError("Renderer is null. Cannot create texture.");
      texture = getFallbackTexture();
    }

    cursors[state] = cursor;
    cursorTextures[state] = texture ? texture : getFallbackTexture();
  }

  void CursorHandler::setCursorState(CursorState state) {
    if (cursors.find(state) != cursors.end() && cursors[state]) {
      SDL_SetCursor(cursors[state]);
      currentState = state;
    } else {
      logsManager.logWarning("Cursor state not found: " + std::to_string(static_cast<int>(state)));
    }
  }

  void CursorHandler::setRenderer(SDL_Renderer* renderer) {
    this->renderer = renderer;
  }

  CursorState CursorHandler::getCursorState() const {
    return currentState;
  }

  SDL_Texture* CursorHandler::getCursorTexture(CursorState state) const {
    if (cursorTextures.empty()) {
      logsManager.logWarning("Cursor Textures Map is empty. No textures have been loaded.");
    }

    auto it = cursorTextures.find(state);
    if (it != cursorTextures.end() && it->second) {
      return it->second;
    } else {
      logsManager.logWarning("Cursor texture for state " + std::to_string(static_cast<int>(state)) + " is null or not found.");
      return nullptr;
    }
  }

  void CursorHandler::resetToDefaultCursor() {
    auto it = cursorTextures.find(CursorState::DEFAULT);

    if (it != cursorTextures.end()) {
      currentCursorTexture = it->second;
    } else {
      logsManager.logError("Default cursor texture not found.");
    }

    currentState = CursorState::DEFAULT;
    SDL_SetCursor(cursors[CursorState::DEFAULT]);
  }

  void CursorHandler::cleanup() {
    ResourceCleaner::cleanupMap(cursorCache, [this](SDL_Cursor* cursor) {
      if (cursor && cursor != defaultCursor) {
        SDL_FreeCursor(cursor);
      }
    });

    if (defaultCursor) {
      SDL_FreeCursor(defaultCursor);
      defaultCursor = nullptr;
    }
    cursors.clear();

    ResourceCleaner::cleanupMap(textureCache, [](SDL_Texture* texture) {
      if (texture) {
        SDL_DestroyTexture(texture);
      }
    });

    cursorTextures.clear();
    if (fallbackTexture) {
      SDL_DestroyTexture(fallbackTexture);
      fallbackTexture = nullptr;
    }
    currentCursorTexture = nullptr;
  }

  SDL_Texture* CursorHandler::getFallbackTexture() {
    if (fallbackTexture) {
      return fallbackTexture;
    }

    if (!renderer) {
      logsManager.logError("Renderer is null. Cannot create fallback cursor texture.");
      return nullptr;
    }

    SDL_Surface* surface = SDL_CreateRGBSurfaceWithFormat(0, Constants::INDEX_TWO, Constants::INDEX_TWO, Constants::BIT_32, SDL_PIXELFORMAT_RGBA32);
    if (!surface) {
      logsManager.logError(std::string("Failed to create fallback cursor surface: ") + SDL_GetError());
      return nullptr;
    }

    Uint32 magenta = SDL_MapRGBA(surface->format, Project::Libraries::Constants::BIT_255, 0, Project::Libraries::Constants::BIT_255, Project::Libraries::Constants::BIT_255);
    Uint32 black = SDL_MapRGBA(surface->format, 0, 0, 0, Project::Libraries::Constants::BIT_255);
    Uint32* pixels = static_cast<Uint32*>(surface->pixels);
    pixels[Project::Libraries::Constants::INDEX_ZERO] = magenta;
    pixels[Project::Libraries::Constants::INDEX_ONE] = black;
    pixels[Project::Libraries::Constants::INDEX_TWO] = black;
    pixels[Project::Libraries::Constants::INDEX_THREE] = magenta;

    fallbackTexture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    if (!fallbackTexture) {
      logsManager.logError(std::string("Failed to create fallback cursor texture: ") + SDL_GetError());
    }
    return fallbackTexture;
  }
}
