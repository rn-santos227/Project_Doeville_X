#ifndef BUTTON_COMPONENT_H
#define BUTTON_COMPONENT_H

#include "ButtonData.h"

#include <string>

#include <SDL.h>
#include <SDL_ttf.h>

#include "components/BaseComponent.h"
#include "components/PositionableComponent.h"
#include "handlers/input/CursorHandler.h"
#include "handlers/input/MouseHandler.h"
#include "interfaces/style_interface/Stylable.h"
#include "libraries/constants/NumericConstants.h"
#include "utilities/config_reader/ConfigReader.h"

namespace Project { namespace Entities { class Entity; } }
namespace Project { namespace Handlers { class MouseHandler; class CursorHandler; } }

namespace Project::Components {
  class ButtonComponent : public BaseComponent, public PositionableComponent, public Project::Interfaces::Stylable {
  public:
    ButtonComponent(
      SDL_Renderer* renderer,
      Project::Utilities::LogsManager& logsManager,
      Project::Utilities::ConfigReader& configReader,
      Project::Handlers::MouseHandler* mouseHandler,
      Project::Handlers::CursorHandler* cursorHandler
    );

    ~ButtonComponent() override;

    Project::Entities::Entity* getOwner() const override { return owner; }
    ComponentType getType() const override { return ComponentType::BUTTON; }

    void update(float deltaTime) override;
    void render() override;
    void build(Project::Utilities::LuaStateWrapper& luaStateWrapper, const std::string& tableName) override;
    void applyStyle() override;

    void setEntityPosition(float x, float y) override;
    void setSize(int w, int h);
    
    void setColor(SDL_Color _color) { data.color = _color; }
    void setHoverColor(SDL_Color _color) { data.hoverColor = _color; }

    void setFontColor(SDL_Color _color);
    void setFontHoverColor(SDL_Color _color);

    void setBorderColor(SDL_Color _color) { data.borderColor = _color; }
    void setBorderWidth(int _width) { data.borderWidth = _width; }
    
    void setCallback(const std::string& functionName) {data.luaFunction = functionName; }
    void setEntityReference(Project::Entities::Entity* entity) { owner = entity; }

    const SDL_Rect& getRect() const { return data.rect; }
  
  private:
    SDL_Renderer* renderer = nullptr;
    ButtonData data;

    Project::Entities::Entity* owner = nullptr;
    Project::Handlers::CursorHandler* cursorHandler = nullptr;
    Project::Handlers::MouseHandler* mouseHandler = nullptr;

    SDL_Texture* textTexture = nullptr;
    TTF_Font* font = nullptr;
    
    Project::Utilities::ConfigReader& configReader;

    void createTextTexture(SDL_Color colorToUse);
  };
}

#endif
