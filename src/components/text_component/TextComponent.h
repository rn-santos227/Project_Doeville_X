#ifndef TEXT_COMPONENT_H
#define TEXT_COMPONENT_H

#include "TextData.h"

#include "components/BaseComponent.h"
#include "components/PositionableComponent.h"
#include "components/TextureHolder.h"

#include <string>

#include <SDL.h>
#include <SDL_ttf.h>

#include "handlers/animation/AnimationHandler.h"
#include "interfaces/style_interface/Stylable.h"
#include "services/styling/Style.h"
#include "services/styling/StyleManager.h"
#include "utilities/logs_manager/LogsManager.h"
#include "utilities/config_reader/ConfigReader.h"

namespace Project::Components {
  class TextComponent : public BaseComponent, public PositionableComponent, public TextureHolder, public Project::Interfaces::Stylable {
  public:
    TextComponent(SDL_Renderer* renderer, Project::Utilities::ConfigReader& configReader, Project::Utilities::LogsManager& logsManager);
    ~TextComponent() override;

    ComponentType getType() const override { return ComponentType::TEXT; }

    void update(float deltaTime) override;
    void render() override;
    void build(Project::Utilities::LuaStateWrapper& luaStateWrapper, const std::string& tableName) override;
    void applyStyle() override;
    
    void setEntityPosition(float x, float y) override;

    void setText(const std::string& newText);
    void setColor(SDL_Color newColor);
    void setColorHex(const std::string& hex);
    void setPosition(int x, int y);
    void setBorderColor(SDL_Color _color) { data.borderColor = _color; }
    void setBorderWidth(int _width) { data.borderWidth = _width; }

    const SDL_Rect& getRect() const { return data.rect; }
    int getWidth() const { return data.rect.w; }
    int getHeight() const { return data.rect.h; }

    void addAnimation(const std::string& name, Project::Handlers::Animation animation);
    void playAnimation(const std::string& name);

  private:
    Project::Handlers::AnimationHandler animationHandler;
    Project::Services::Style style{};
    TextData data;
    
    Project::Utilities::ConfigReader& configReader;

    SDL_Renderer* renderer;
    TTF_Font* font;

    void createTexture();
  };
}

#endif
