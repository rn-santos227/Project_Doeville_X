#ifndef BUTTON_COMPONENT_H
#define BUTTON_COMPONENT_H

#include <string>

#include <SDL.h>

#include "components/BaseComponent.h"
#include "components/PositionableComponent.h"
#include "handlers/input/CursorHandler.h"
#include "handlers/input/MouseHandler.h"
#include "interfaces/style_interface/Stylable.h"

namespace Project { namespace Entities { class Entity; } }
namespace Project { namespace Handlers { class MouseHandler; class CursorHandler; } }

namespace Project::Components {
  class ButtonComponent : public BaseComponent, public PositionableComponent, public Project::Interfaces::Stylable {
  public:
    ButtonComponent(
      SDL_Renderer* renderer,
      Project::Utilities::LogsManager& logsManager,
      Project::Handlers::MouseHandler* mouseHandler,
      Project::Handlers::CursorHandler* cursorHandler
    );

    void update(float deltaTime) override;
    void render() override;
    void build(Project::Utilities::LuaStateWrapper& luaStateWrapper, const std::string& tableName) override;
    void applyStyle() override;

    void setEntityPosition(int x, int y) override;
    void setSize(int w, int h);
    
    void setColor(SDL_Color color);
    void setHoverColor(SDL_Color color);
    
    void setCallback(const std::string& functionName);
    void setEntityReference(Project::Entities::Entity* entity) { owner = entity; }

    const SDL_Rect& getRect() const { return rect; }
  
  private:
    SDL_Renderer* renderer = nullptr;

    Project::Entities::Entity* owner = nullptr;
    Project::Handlers::CursorHandler* cursorHandler = nullptr;
    Project::Handlers::MouseHandler* mouseHandler = nullptr;
    
    SDL_Rect rect{0,0,0,0};
    SDL_Color color{255,255,255,255};
    SDL_Color hoverColor{255,255,255,255};

    std::string luaFunction;
    bool hovered = false;
    bool wasPressed = false;
  };
}

#endif
