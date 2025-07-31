#ifndef INPUT_COMPONENT_H
#define INPUT_COMPONENT_H

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

    void setEntityPosition(int x, int y) override;

    const std::string& getText() const { return currentText; }
    void setText(const std::string& text);

    const SDL_Rect& getRect() const { return rect; }
    int getWidth() const { return textureW; }
    int getHeight() const { return textureH; }

  private:
    SDL_Renderer* renderer;
    Project::Utilities::ConfigReader& configReader;
    Project::Handlers::MouseHandler* mouseHandler;
    Project::Handlers::CursorHandler* cursorHandler;

    std::array<Uint8, SDL_NUM_SCANCODES> prevKeys{};

    TTF_Font* font;
    SDL_Rect rect{0,0,0,0};
    SDL_Color bgColor{0,0,0,255};
    SDL_Color textColor = Project::Libraries::Constants::COLOR_WHITE;
    SDL_Color borderColor{0,0,0,0};

    std::string fontPath;
    std::string currentText;
    std::string placeholder;

    float backspaceTimer{0.0f};
    float cursorBlink{0.0f};

    int fontSize = Project::Libraries::Constants::DEFAULT_BASE_FONT_SIZE;
    int borderWidth{0};
    int textOffset{0};
    int textureW{0};
    int textureH{0};

    std::size_t maxChars = Project::Libraries::Constants::DEFAULT_MAX_CHARS;
    std::size_t caretPos{0};

    bool activeInput{false};
    bool backspaceHeld{false};    
    bool hovered{false};
    bool mousePrev{false};
    bool showCaret{true};

    void createTexture();
    void processInput(const Uint8* state, float deltaTime);
  };
}

#endif
