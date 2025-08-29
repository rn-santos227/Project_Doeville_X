#ifndef INPUT_COMPONENT_H
#define INPUT_COMPONENT_H

#include "InputData.h"
#include "InputType.h"

#include <array>
#include <string>

#include <SDL.h>
#include <SDL_ttf.h>

#include "components/BaseComponent.h"
#include "components/PositionableComponent.h"
#include "components/TextureHolder.h"
#include "interfaces/style_interface/Stylable.h"
#include "handlers/input/MouseHandler.h"
#include "handlers/input/CursorHandler.h"
#include "libraries/constants/Constants.h"
#include "utilities/logs_manager/LogsManager.h"
#include "utilities/config_reader/ConfigReader.h"

namespace Project::Components {
  class InputComponent : 
    public BaseComponent, 
    public PositionableComponent,
    public TextureHolder, 
    public Project::Interfaces::Stylable {
  public:
    InputComponent(
      SDL_Renderer* renderer,
      Project::Utilities::LogsManager& logsManager,
      Project::Utilities::ConfigReader& configReader,
      Project::Handlers::MouseHandler* mouseHandler,
      Project::Handlers::CursorHandler* cursorHandler
    );
    ~InputComponent() override;

    ComponentType getType() const override { return ComponentType::INPUT; }

    void update(float deltaTime) override;
    void render() override;
    void build(Project::Utilities::LuaStateWrapper& luaStateWrapper, const std::string& tableName) override;
    void applyStyle() override;

    void setEntityPosition(float x, float y) override;

    const std::string& getText() const { return data.currentText; }
    void setText(const std::string& text);

    const SDL_Rect& getRect() const { return data.rect; }
    int getWidth() const { return data.textureW; }
    int getHeight() const { return data.textureH; }

    void setInputType(InputType _type) { data.inputType = _type; }
    InputType getInputType() const { return data.inputType; }

  private:
    SDL_Renderer* renderer;
    Project::Utilities::ConfigReader& configReader;
    Project::Handlers::MouseHandler* mouseHandler;
    Project::Handlers::CursorHandler* cursorHandler;

    InputData data;
    TTF_Font* font;

    void createTexture();
    void processInput(const Uint8* state, float deltaTime);
  };
}

#endif
