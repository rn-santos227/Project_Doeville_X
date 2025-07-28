#ifndef FONT_HANDLER_H
#define FONT_HANDLER_H

#include <iostream>
#include <string>
#include <unordered_map>

#include <SDL_ttf.h>
#include <SDL.h>

#include "interfaces/cleanup_interface/Cleanable.h"
#include "utilities/logs_manager/LogsManager.h"

namespace Project::Handlers {
  class FontHandler : public Project::Interfaces::Cleanable {
  public:
    FontHandler(Project::Utilities::LogsManager& logsManager);
    ~FontHandler();

    void cleanup() override;

    bool loadFont(const std::string& fontId, const std::string& path, int fontSize);
    SDL_Texture* renderText(SDL_Renderer* renderer, const std::string& text, const std::string& fontId, SDL_Color color);

  private:
    Project::Utilities::LogsManager& logsManager;

    std::unordered_map<std::string, TTF_Font*> fonts;
    std::unordered_map<SDL_Renderer*, SDL_Texture*> fallbackTextures;
    
    SDL_Texture* getFallbackTexture(SDL_Renderer* renderer);
  };
}

#endif
