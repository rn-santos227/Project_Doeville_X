#ifndef BUTTON_COMPONENT_H
#define BUTTON_COMPONENT_H

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

    void update(float deltaTime) override;
    void render() override;
    void build(Project::Utilities::LuaStateWrapper& luaStateWrapper, const std::string& tableName) override;
    void applyStyle() override;

    void setEntityPosition(int x, int y) override;
    void setSize(int w, int h);
    
    void setColor(SDL_Color color);
    void setHoverColor(SDL_Color color);

    void setFontColor(SDL_Color color);
    void setFontHoverColor(SDL_Color color);
    
    void setCallback(const std::string& functionName);
    void setEntityReference(Project::Entities::Entity* entity) { owner = entity; }

    const SDL_Rect& getRect() const { return rect; }
  
  private:
    SDL_Renderer* renderer = nullptr;

    Project::Entities::Entity* owner = nullptr;
    Project::Handlers::CursorHandler* cursorHandler = nullptr;
    Project::Handlers::MouseHandler* mouseHandler = nullptr;

    SDL_Texture* textTexture = nullptr;
    TTF_Font* font = nullptr;
    
    Project::Utilities::ConfigReader& configReader;
    
    SDL_Rect rect{0,0,0,0};
    SDL_Color color{
      Project::Libraries::Constants::DEFAULT_MAX_CHANNEL,
      Project::Libraries::Constants::DEFAULT_MAX_CHANNEL,
      Project::Libraries::Constants::DEFAULT_MAX_CHANNEL,
      Project::Libraries::Constants::DEFAULT_MAX_CHANNEL
    };

    SDL_Color hoverColor{
      Project::Libraries::Constants::DEFAULT_MAX_CHANNEL,
      Project::Libraries::Constants::DEFAULT_MAX_CHANNEL,
      Project::Libraries::Constants::DEFAULT_MAX_CHANNEL,
      Project::Libraries::Constants::DEFAULT_MAX_CHANNEL
    };
    
    SDL_Color fontColor{
      Project::Libraries::Constants::DEFAULT_MAX_CHANNEL,
      Project::Libraries::Constants::DEFAULT_MAX_CHANNEL,
      Project::Libraries::Constants::DEFAULT_MAX_CHANNEL,
      Project::Libraries::Constants::DEFAULT_MAX_CHANNEL
    };
    
    SDL_Color fontHoverColor{
      Project::Libraries::Constants::DEFAULT_MAX_CHANNEL,
      Project::Libraries::Constants::DEFAULT_MAX_CHANNEL,
      Project::Libraries::Constants::DEFAULT_MAX_CHANNEL,
      Project::Libraries::Constants::DEFAULT_MAX_CHANNEL
    };
    
    SDL_Rect textRect{0,0,0,0};
    
    std::string luaFunction;
    std::string text;
    
    int paddingTop{0};
    int paddingRight{0};
    int paddingBottom{0};
    int paddingLeft{0};

    int marginTop{0};
    int marginRight{0};
    int marginBottom{0};
    int marginLeft{0};

    int fontSize = 16;

    bool hovered = false;
    bool wasPressed = false;

    void createTextTexture(SDL_Color colorToUse);
  };
}

#endif
