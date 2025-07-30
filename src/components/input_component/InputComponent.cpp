#include "InputComponent.h"

#include "handlers/input/MouseHandler.h"
#include "handlers/input/CursorHandler.h"
#include "libraries/constants/Constants.h"
#include "libraries/keys/Keys.h"
#include "services/styling/StyleManager.h"
#include "utilities/color/ColorUtils.h"

namespace Project::Components {
  using Project::Utilities::LogsManager;
  using Project::Utilities::ConfigReader;
  using Project::Utilities::ColorUtils;
  using Project::Handlers::CursorHandler;
  using Project::Handlers::MouseHandler;
  using Project::Services::StyleManager;

  namespace Constants = Project::Libraries::Constants;
  namespace Keys = Project::Libraries::Keys;

  InputComponent::InputComponent(SDL_Renderer* renderer, LogsManager& logsManager, ConfigReader& configReader,
   MouseHandler* mouseHandler, CursorHandler* cursorHandler)
    : BaseComponent(logsManager), renderer(renderer), configReader(configReader),
      mouseHandler(mouseHandler), cursorHandler(cursorHandler), font(nullptr) {}

  InputComponent::~InputComponent() {
    destroyTexture();
    textureW = 0;
    textureH = 0;
    if (font) {
      TTF_CloseFont(font);
      font = nullptr;
    }
  }

  void InputComponent::update(float deltaTime) {
    if (!mouseHandler || !cursorHandler) return;

    int mx = mouseHandler->getMouseX();
    int my = mouseHandler->getMouseY();
    SDL_Point p{mx, my};
    bool inside = SDL_PointInRect(&p, &rect);
    bool mouseDown = mouseHandler->isButtonDown(SDL_BUTTON_LEFT);

    if (inside) {
      if (!hovered) {
        hovered = true;
        cursorHandler->setCursorState(Project::Handlers::CursorState::TEXT);
      }
      if (mouseDown && !mousePrev) {
        activeInput = true;
        cursorBlink = 0.0f;
        showCaret = true;
      }
    } else {
      if (hovered) {
        hovered = false;
        cursorHandler->setCursorState(Project::Handlers::CursorState::DEFAULT);
      }
      if (mouseDown && !mousePrev) {
        activeInput = false;
      }
    }

    mousePrev = mouseDown;

    if (activeInput) {
      cursorBlink += deltaTime;
      if (cursorBlink >= 0.5f) {
        showCaret = !showCaret;
        cursorBlink = 0.0f;
      }

      const Uint8* state = SDL_GetKeyboardState(nullptr);
      processInput(state);
      std::copy(state, state + SDL_NUM_SCANCODES, prevKeys.begin());
    }
  }

  void InputComponent::render() {
    if (!renderer) return;
    SDL_SetRenderDrawColor(renderer, bgColor.r, bgColor.g, bgColor.b, bgColor.a);
    SDL_RenderFillRect(renderer, &rect);

    if (borderWidth > 0) {
      SDL_SetRenderDrawColor(renderer, borderColor.r, borderColor.g, borderColor.b, borderColor.a);
      for (int i = 0; i < borderWidth; ++i) {
        SDL_Rect b{rect.x + i, rect.y + i, rect.w - 2 * i, rect.h - 2 * i};
        SDL_RenderDrawRect(renderer, &b);
      }
    }
  }

  void InputComponent::build(Project::Utilities::LuaStateWrapper& luaStateWrapper, const std::string& tableName) {
    rect.w = static_cast<int>(luaStateWrapper.getTableNumber(tableName, Keys::WIDTH, Constants::DEFAULT_COMPONENT_SIZE));
    rect.h = static_cast<int>(luaStateWrapper.getTableNumber(tableName, Keys::HEIGHT, Constants::DEFAULT_COMPONENT_SIZE));

    std::string colorHex = luaStateWrapper.getTableString(tableName, Keys::COLOR_HEX, Constants::DEFAULT_SHAPE_COLOR_HEX);
    Uint8 alpha = static_cast<Uint8>(luaStateWrapper.getTableNumber(tableName, Keys::COLOR_ALPHA, Constants::FULL_ALPHA));
    bgColor = ColorUtils::hexToRGB(colorHex, alpha);

    placeholder = luaStateWrapper.getTableString(tableName, Keys::TEXT, Constants::EMPTY_STRING);
    currentText.clear();

    std::string defaultFontPath = configReader.getValue(Keys::FONT_SECTION, Keys::FONT_DEFAULT_PATH, Constants::DEFAULT_FONT_PATH);
    fontPath = luaStateWrapper.getTableString(tableName, Keys::FONT_PATH, defaultFontPath);

    int defaultFontSize = configReader.getIntValue(Keys::FONT_SECTION, Keys::FONT_DEFAULT_SIZE, Constants::DEFAULT_FONT_SIZE);
    fontSize = static_cast<int>(luaStateWrapper.getTableNumber(tableName, Keys::FONT_SIZE, static_cast<float>(defaultFontSize)));
    font = TTF_OpenFont(fontPath.c_str(), fontSize);

    if (!font) {
      logsManager.logWarning(std::string("Failed to load font: ") + fontPath + ". Using fallback font.");
      font = TTF_OpenFont(Constants::DEFAULT_FONT_PATH, fontSize);
      if (!font) {
        logsManager.logError(std::string("Failed to load fallback font: ") + Constants::DEFAULT_FONT_PATH);
      }
    }

    std::string fontColorHex = luaStateWrapper.getTableString(tableName, Keys::FONT_COLOR_HEX, Constants::DEFAULT_SHAPE_COLOR_HEX);
    textColor = ColorUtils::hexToRGB(fontColorHex, Constants::FULL_ALPHA);

    createTexture();
  }
}
