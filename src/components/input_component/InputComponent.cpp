#include "InputComponent.h"
#include "InputTypeResolver.h"

#include <algorithm>

#include "handlers/input/MouseHandler.h"
#include "handlers/input/CursorHandler.h"
#include "libraries/categories/InputCategories.h"
#include "libraries/constants/Constants.h"
#include "libraries/keys/Keys.h"
#include "services/styling/StyleManager.h"
#include "utilities/color/ColorUtils.h"
#include "utilities/geometry/GeometryUtils.h"

namespace Project::Components {
  using Project::Utilities::LogsManager;
  using Project::Utilities::ConfigReader;
  using Project::Utilities::ColorUtils;
  using Project::Handlers::CursorHandler;
  using Project::Handlers::MouseHandler;
  using Project::Services::StyleManager;

  namespace Constants = Project::Libraries::Constants;
  namespace Keys = Project::Libraries::Keys;

  InputComponent::InputComponent(SDL_Renderer* renderer, 
    LogsManager& logsManager, ConfigReader& configReader,
    MouseHandler* mouseHandler, CursorHandler* cursorHandler)
    : BaseComponent(logsManager), renderer(renderer), configReader(configReader),
      mouseHandler(mouseHandler), cursorHandler(cursorHandler), font(nullptr) {}

  InputComponent::~InputComponent() {
    destroyTexture();
    data.textureW = 0;
    data.textureH = 0;
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
    bool inside = SDL_PointInRect(&p, &data.rect);
    bool mouseDown = mouseHandler->isButtonDown(SDL_BUTTON_LEFT);

    if (inside) {
      if (!data.hovered) {
        data.hovered = true;
        cursorHandler->setCursorState(Project::Handlers::CursorState::TEXT);
      }
      if (mouseDown && !data.mousePrev) {
        data.activeInput = true;
        data.cursorBlink = 0.0f;
        data.showCaret = true;
      }
    } else {
      if (data.hovered) {
        data.hovered = false;
        cursorHandler->setCursorState(Project::Handlers::CursorState::DEFAULT);
      }
      if (mouseDown && !data.mousePrev) {
        data.activeInput = false;
      }
    }

    data.mousePrev = mouseDown;

    if (data.activeInput) {
      data.cursorBlink += deltaTime;
      if (data.cursorBlink >= 0.5f) {
        data.showCaret = !data.showCaret;
        data.cursorBlink = 0.0f;
      }

      const Uint8* state = SDL_GetKeyboardState(nullptr);
      processInput(state, deltaTime);
      std::copy(state, state + SDL_NUM_SCANCODES, data.prevKeys.begin());
    }
  }

  void InputComponent::render() {
    if (!renderer) return;
    SDL_SetRenderDrawColor(renderer, data.bgColor.r, data.bgColor.g, data.bgColor.b, data.bgColor.a);
    if (data.borderRadius > 0) {
      SDL_FRect rectF{
        static_cast<float>(data.rect.x),
        static_cast<float>(data.rect.y),
        static_cast<float>(data.rect.w),
        static_cast<float>(data.rect.h)
      };
      
      Project::Utilities::GeometryUtils::renderFilledRoundedRect(renderer, rectF, data.borderRadius);
    } else {
      SDL_RenderFillRect(renderer, &data.rect);
    }

    if (data.borderWidth > 0) {
      SDL_SetRenderDrawColor(renderer, data.borderColor.r, data.borderColor.g, data.borderColor.b, data.borderColor.a);
      for (int i = 0; i < data.borderWidth; ++i) {
        SDL_Rect b{data.rect.x + i, data.rect.y + i, data.rect.w - Constants::INDEX_TWO * i, data.rect.h - Constants::INDEX_TWO * i};
        if (data.borderRadius > 0) {
          int radius = std::max(0, data.borderRadius - i);
          SDL_FRect bF{
            static_cast<float>(b.x),
            static_cast<float>(b.y),
            static_cast<float>(b.w),
            static_cast<float>(b.h)
          };
          
          Project::Utilities::GeometryUtils::renderRoundedRect(renderer, bF, radius);
        } else {
          SDL_RenderDrawRect(renderer, &b);
        }
      }
    }

    int textX = data.rect.x + data.paddingLeft;
    int viewWidth = data.rect.w - data.paddingLeft - data.paddingRight;
    if (texture) {
      int caretPixels = 0;
      if (font && data.caretPos > 0) {
        std::string left = data.currentText.substr(0, data.caretPos);
        TTF_SizeText(font, left.c_str(), &caretPixels, nullptr);
      }
      if (data.textureW <= viewWidth) {
        data.textOffset = 0;
      } else {
        if (caretPixels - data.textOffset > viewWidth) data.textOffset = caretPixels - viewWidth;
        else if (caretPixels - data.textOffset < 0) data.textOffset = caretPixels;
        if (data.textOffset < 0) data.textOffset = 0;
        if (data.textOffset > data.textureW - viewWidth) data.textOffset = data.textureW - viewWidth;
      }

      SDL_Rect src = {data.textOffset, 0, std::min(viewWidth, data.textureW - data.textOffset), data.textureH};
      SDL_Rect dst = {textX, data.rect.y + data.paddingTop + (data.rect.h - data.paddingTop - data.paddingBottom - src.h) / Constants::INDEX_TWO, src.w, src.h};
      SDL_RenderCopy(renderer, texture, &src, &dst);
    } else if (!data.placeholder.empty() && font) {
      SDL_Color phColor = {data.textColor.r, data.textColor.g, data.textColor.b, static_cast<Uint8>(data.textColor.a / Constants::INDEX_TWO)};
      SDL_Surface* surface = TTF_RenderText_Blended(font, data.placeholder.c_str(), phColor);
      if (surface) {
        SDL_Texture* tmp = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_Rect dst = {textX, data.rect.y + data.paddingTop + (data.rect.h - data.paddingTop - data.paddingBottom - surface->h) / Constants::INDEX_TWO, surface->w, surface->h};
        SDL_RenderCopy(renderer, tmp, nullptr, &dst);
        SDL_DestroyTexture(tmp);
        SDL_FreeSurface(surface);
      }
    }
    if (data.activeInput && data.showCaret) {
      SDL_SetRenderDrawColor(renderer, data.textColor.r, data.textColor.g, data.textColor.b, data.textColor.a);
      int top = data.rect.y + data.paddingTop;
      int bottom = data.rect.y + data.rect.h - data.paddingBottom;
      int offset = 0;
      if (font && data.caretPos > 0) {
        std::string left = data.currentText.substr(0, data.caretPos);
        TTF_SizeText(font, left.c_str(), &offset, nullptr);
      }
      int caretX = textX + offset - data.textOffset + Constants::INDEX_TWO;
      SDL_RenderDrawLine(renderer, caretX, top, caretX, bottom);
    }
  }

  void InputComponent::build(Project::Utilities::LuaStateWrapper& luaStateWrapper, const std::string& tableName) {
    data.rect.w = static_cast<int>(luaStateWrapper.getTableNumber(tableName, Keys::WIDTH, Constants::DEFAULT_COMPONENT_SIZE));
    data.rect.h = static_cast<int>(luaStateWrapper.getTableNumber(tableName, Keys::HEIGHT, Constants::DEFAULT_COMPONENT_SIZE));

    std::string colorHex = luaStateWrapper.getTableString(tableName, Keys::COLOR_HEX, Constants::DEFAULT_SHAPE_COLOR_HEX);
    Uint8 alpha = static_cast<Uint8>(luaStateWrapper.getTableNumber(tableName, Keys::COLOR_ALPHA, Constants::FULL_ALPHA));
    data.bgColor = ColorUtils::hexToRGB(colorHex, alpha);

    data.placeholder = luaStateWrapper.getTableString(tableName, Keys::TEXT, Constants::EMPTY_STRING);
    data.currentText.clear();

    std::string typeStr = luaStateWrapper.getTableString(tableName, Keys::TYPE, Project::Libraries::Categories::Inputs::TEXT);
    setInputType(InputTypeResolver::resolve(typeStr));

    data.caretPos = 0;
    data.textOffset = 0;
    data.maxChars = static_cast<std::size_t>(luaStateWrapper.getTableNumber(tableName, Keys::MAX_CHARS, Constants::DEFAULT_MAX_CHARS));

    std::string defaultFontPath = configReader.getValue(Keys::FONT_SECTION, Keys::FONT_DEFAULT_PATH, Constants::DEFAULT_FONT_PATH);
    data.fontPath = luaStateWrapper.getTableString(tableName, Keys::FONT_PATH, defaultFontPath);
    
    int defaultFontSize = configReader.getIntValue(Keys::FONT_SECTION, Keys::FONT_DEFAULT_SIZE, Constants::DEFAULT_FONT_SIZE);
    data.fontSize = static_cast<int>(luaStateWrapper.getTableNumber(tableName, Keys::FONT_SIZE, static_cast<float>(defaultFontSize)));
    font = TTF_OpenFont(data.fontPath.c_str(), data.fontSize);
    
    if (!font) {
      logsManager.logWarning(std::string("Failed to load font: ") + data.fontPath + ". Using fallback font.");
      font = TTF_OpenFont(Constants::DEFAULT_FONT_PATH,data.fontSize);
      if (!font) {
        logsManager.logError(std::string("Failed to load fallback font: ") + Constants::DEFAULT_FONT_PATH);
      }
    }

    std::string fontColorHex = luaStateWrapper.getTableString(tableName, Keys::FONT_COLOR_HEX, Constants::DEFAULT_SHAPE_COLOR_HEX);
    data.textColor = ColorUtils::hexToRGB(fontColorHex, Constants::FULL_ALPHA);

    createTexture();
  }

  void InputComponent::applyStyle() {
    std::istringstream classes(getClass());
    std::string cls;
    while (classes >> cls) {
      std::string selector = "." + cls;
      Project::Services::Style s = StyleManager::getInstance().getStyle(selector);
      if (s.width > 0) data.rect.w = s.width;
      if (s.height > 0) data.rect.h = s.height;
      if (s.background.a != 0) data.bgColor = s.background;
      if (s.borderColor.a != 0) data.borderColor = s.borderColor;
      if (s.borderWidth > 0) data.borderWidth = s.borderWidth;
      if (s.borderRadius > 0) data.borderRadius = static_cast<int>(s.borderRadius);
      if (s.fontColor.a != 0) data.textColor = s.fontColor;
      if (s.fontSize > 0 && data.fontPath.size() > 0) {
        if (font) TTF_CloseFont(font);
        data.fontSize = s.fontSize;
        font = TTF_OpenFont(data.fontPath.c_str(), data.fontSize);
        if (!font) {
          logsManager.logWarning(std::string("Failed to load font: ") + data.fontPath + ". Using fallback font.");
          font = TTF_OpenFont(Project::Libraries::Constants::DEFAULT_FONT_PATH, data.fontSize);
        }
      }

      if (s.paddingTop || s.paddingRight || s.paddingBottom || s.paddingLeft) {
        data.paddingTop = s.paddingTop;
        data.paddingRight = s.paddingRight;
        data.paddingBottom = s.paddingBottom;
        data.paddingLeft = s.paddingLeft;
      }
    }
    createTexture();
  }

  void InputComponent::setEntityPosition(float x, float y) {
    data.rect.x = static_cast<int>(x);
    data.rect.y = static_cast<int>(y);
  }

  void InputComponent::setText(const std::string& text) {
    data.currentText = text;
    createTexture();
  }

  void InputComponent::createTexture() {
    destroyTexture();
    data.textureW = 0;
    data.textureH = 0;
    if (!font || data.currentText.empty()) return;

    std::string text = data.currentText;
    if (data.inputType == InputType::PASSWORD) {
      text.assign(data.currentText.size(), '*');
    }
    SDL_Surface* surface = TTF_RenderText_Blended(font, text.c_str(), data.textColor);

    if (!surface) {
      logsManager.logWarning(std::string("Failed to render input text: ") + TTF_GetError());
      return;
    }
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    data.textureW = surface->w;
    data.textureH = surface->h;
    data.rect.w = data.rect.w ? data.rect.w : surface->w + Constants::INDEX_EIGHT;
    data.rect.h = data.rect.h ? data.rect.h : surface->h + Constants::INDEX_EIGHT;
    SDL_FreeSurface(surface);
  }

  void InputComponent::processInput(const Uint8* state, float deltaTime) {
   bool shift = state[SDL_SCANCODE_LSHIFT] || state[SDL_SCANCODE_RSHIFT];
    bool textChanged = false;

    auto wasPressed = [&](SDL_Scancode sc){ return state[sc] && !data.prevKeys[sc]; };

    bool bsDown = state[SDL_SCANCODE_BACKSPACE];
    if (wasPressed(SDL_SCANCODE_BACKSPACE)) {
      if (data.caretPos > 0 && !data.currentText.empty()) {
        data.currentText.erase(data.caretPos-1, 1);
        --data.caretPos;
        textChanged = true;
      }
      data.backspaceHeld = true;
      data.backspaceTimer = 0.4f;
    } else if (bsDown && data.backspaceHeld) {
      data.backspaceTimer -= deltaTime;
      if (data.backspaceTimer <= 0.0f) {
        if (data.caretPos > 0 && !data.currentText.empty()) {
          data.currentText.erase(data.caretPos-1, 1);
          --data.caretPos;
          textChanged = true;
        }
        data.backspaceTimer = Constants::DEFAULT_HALF;
      }
    }

    if (!bsDown) {
      data.backspaceHeld = false;
      data.backspaceTimer = 0.0f;
    }

    if (wasPressed(SDL_SCANCODE_SPACE) && data.currentText.size() < data.maxChars && data.inputType != InputType::NUMERIC) {
      data.currentText.insert(data.caretPos, 1, ' ');
      ++data.caretPos;
      textChanged = true;
    }

    if (data.inputType != InputType::NUMERIC) {
      for (int i = SDL_SCANCODE_A; i <= SDL_SCANCODE_Z; ++i) {
        if (wasPressed(static_cast<SDL_Scancode>(i)) && data.currentText.size() < data.maxChars) {
          char c = static_cast<char>('a' + (i - SDL_SCANCODE_A));
          if (shift) c = static_cast<char>(toupper(c));
          data.currentText.insert(data.caretPos, 1, c);
          ++data.caretPos;
          textChanged = true;
        }
      }
    }

    for (int i = SDL_SCANCODE_0; i <= SDL_SCANCODE_9; ++i) {
      if (wasPressed(static_cast<SDL_Scancode>(i)) && data.currentText.size() < data.maxChars) {
        char c = static_cast<char>('0' + (i - SDL_SCANCODE_0));
        static const char shiftedDigits[10] = {')','!','@','#','$','%','^','&','*','('};
        if (shift && data.inputType != InputType::NUMERIC) {
          c = shiftedDigits[i - SDL_SCANCODE_0];
        }
        data.currentText.insert(data.caretPos, 1, c);
        ++data.caretPos;
        textChanged = true;
      }
    }

    if (data.inputType != InputType::NUMERIC) {
      struct KeyMap { SDL_Scancode sc; char normal; char shifted; };
      static const KeyMap keys[] = {
        {SDL_SCANCODE_MINUS,'-','_'},
        {SDL_SCANCODE_EQUALS,'=','+'},
        {SDL_SCANCODE_LEFTBRACKET,'[','{'},
        {SDL_SCANCODE_RIGHTBRACKET,']','}'},
        {SDL_SCANCODE_BACKSLASH,'\\','|'},
        {SDL_SCANCODE_SEMICOLON,';',':'},
        {SDL_SCANCODE_APOSTROPHE,'\'','"'},
        {SDL_SCANCODE_GRAVE,'`','~'},
        {SDL_SCANCODE_COMMA,',','<'},
        {SDL_SCANCODE_PERIOD,'.','>'},
        {SDL_SCANCODE_SLASH,'/','?'}
      };
      for (const auto& k : keys) {
        if (wasPressed(k.sc) && data.currentText.size() < data.maxChars) {
          char c = shift ? k.shifted : k.normal;
          data.currentText.insert(data.caretPos, 1, c);
          ++data.caretPos;
          textChanged = true;
        }
      }
    }

    if (wasPressed(SDL_SCANCODE_LEFT)) {
      if (data.caretPos > 0) {
        --data.caretPos;
      }
    }

    if (wasPressed(SDL_SCANCODE_RIGHT)) {
      if (data.caretPos < data.currentText.size()) {
        ++data.caretPos;
      }
    }
    if (textChanged) createTexture();
  }
}
