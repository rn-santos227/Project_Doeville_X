#ifndef TEXT_COMPONENT_H
#define TEXT_COMPONENT_H
#include "BaseComponent.h"

#include <SDL.h>
#include <SDL_ttf.h>
#include <string>

#include "utilities/logs_manager/LogsManager.h"

using namespace Project::Utilities;

namespace Project::Components {
  class TextComponent : public BaseComponent {
  public:
    TextComponent(SDL_Renderer* renderer, TTF_Font* font, SDL_Color color, const std::string& text, const std::string& fontPath, int fontSize, LogsManager& logsManager);
    ~TextComponent();

    void update(float deltaTime) override;
    void render() override;

    void setText(const std::string& newText);
    void setPosition(int x, int y);

  private:
    SDL_Renderer* renderer;
    SDL_Texture* texture;
    TTF_Font* font;

    SDL_Color color;
    SDL_Rect rect;

    std::string currentText;

    void createTexture();
    void freeTexture();
  };
}

#endif