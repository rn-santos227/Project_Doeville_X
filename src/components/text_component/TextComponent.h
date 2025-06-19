#ifndef TEXT_COMPONENT_H
#define TEXT_COMPONENT_H

#include "components/BaseComponent.h"
#include "components/TextureHolder.h"

#include <string>

#include <SDL.h>
#include <SDL_ttf.h>

#include "handlers/animation/AnimationHandler.h"
#include "utilities/logs_manager/LogsManager.h"

using namespace Project::Utilities;
using namespace Project::Handlers;

namespace Project::Components {
  class TextComponent : public BaseComponent, public TextureHolder {
  public:
    TextComponent(SDL_Renderer* renderer, TTF_Font* font, SDL_Color color, const std::string& text, const std::string& fontPath, int fontSize, LogsManager& logsManager);
    ~TextComponent() override;

    void update(float deltaTime) override;
    void render() override;

    void setText(const std::string& newText);
    void setPosition(int x, int y);

    void addAnimation(const std::string& name, Animation animation);
    void playAnimation(const std::string& name);

  private:
    AnimationHandler animationHandler;
    
    SDL_Renderer* renderer;
    TTF_Font* font;

    SDL_Color color;
    SDL_Rect rect;

    std::string currentText;
    std::string fontPath;
    int fontSize;

    void createTexture();
  };
}
#endif
