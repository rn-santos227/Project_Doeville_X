#include "CursorHandler.h"

#include "helpers/resource_cleaner/ResourceCleaner.h"

namespace Project::Handlers {
  using Project::Utilities::LogsManager;
  using Project::Helpers::ResourceCleaner;

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
      if (logsManager.checkAndLogError(!surface, "Failed to load cursor image: " + filePath + " - " + std::string(SDL_GetError()))) {
        return;
      }

      logsManager.logMessage("Successfully loaded cursor image: " + filePath);
      cursor = SDL_CreateColorCursor(surface, hotspotX, hotspotY);
      if (cursor) {
        cursorCache[filePath] = cursor;
      } else {
        logsManager.logWarning("Failed to create SDL cursor.");
      }

      if (renderer) {
        texture = SDL_CreateTextureFromSurface(renderer, surface);
        if (!texture) {
          logsManager.logError("Failed to create texture for cursor: " + filePath + " - " + std::string(SDL_GetError()));
        } else {
          textureCache[filePath] = texture;
        }
      } else {
        logsManager.logError("Renderer is null. Cannot create texture.");
      }
      SDL_FreeSurface(surface);
    }

    if (cursor) {
      cursors[state] = cursor;
    }
    if (texture) {
      cursorTextures[state] = texture;
    }
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
    currentCursorTexture = nullptr;
  }
}
