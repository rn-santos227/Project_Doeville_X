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

  InputComponent::InputComponent(SDL_Renderer* renderer, 
    LogsManager& logsManager, ConfigReader& configReader,
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
      processInput(state, deltaTime);
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
        SDL_Rect b{rect.x + i, rect.y + i, rect.w - Constants::INDEX_TWO * i, rect.h - Constants::INDEX_TWO * i};
        SDL_RenderDrawRect(renderer, &b);
      }
    }

    int textX = rect.x + Constants::INDEX_FOUR;
    int viewWidth = rect.w - Constants::INDEX_EIGHT;
    if (texture) {
      int caretPixels = 0;
      if (font && caretPos > 0) {
        std::string left = currentText.substr(0, caretPos);
        TTF_SizeText(font, left.c_str(), &caretPixels, nullptr);
      }
      if (textureW <= viewWidth) {
        textOffset = 0;
      } else {
        if (caretPixels - textOffset > viewWidth) textOffset = caretPixels - viewWidth;
        else if (caretPixels - textOffset < 0) textOffset = caretPixels;
        if (textOffset < 0) textOffset = 0;
        if (textOffset > textureW - viewWidth) textOffset = textureW - viewWidth;
      }

      SDL_Rect src = {textOffset, 0, std::min(viewWidth, textureW - textOffset), textureH};
      SDL_Rect dst = {textX, rect.y + (rect.h - src.h) / Constants::INDEX_TWO, src.w, src.h};
      SDL_RenderCopy(renderer, texture, &src, &dst);
    } else if (!placeholder.empty() && font) {
      SDL_Color phColor = {textColor.r, textColor.g, textColor.b, static_cast<Uint8>(textColor.a / Constants::INDEX_TWO)};
      SDL_Surface* surface = TTF_RenderText_Blended(font, placeholder.c_str(), phColor);
      if (surface) {
        SDL_Texture* tmp = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_Rect dst = {textX, rect.y + (rect.h - surface->h) / Constants::INDEX_TWO, surface->w, surface->h};
        SDL_RenderCopy(renderer, tmp, nullptr, &dst);
        SDL_DestroyTexture(tmp);
        SDL_FreeSurface(surface);
      }
    }
    if (activeInput && showCaret) {
      SDL_SetRenderDrawColor(renderer, textColor.r, textColor.g, textColor.b, textColor.a);
      int top = rect.y + Constants::INDEX_FOUR;
      int bottom = rect.y + rect.h - Constants::INDEX_FOUR;
      int offset = 0;
      if (font && caretPos > 0) {
        std::string left = currentText.substr(0, caretPos);
        TTF_SizeText(font, left.c_str(), &offset, nullptr);
      }
      int caretX = textX + offset - textOffset + Constants::INDEX_TWO;
      SDL_RenderDrawLine(renderer, caretX, top, caretX, bottom);
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

    caretPos = 0;
    textOffset = 0;
    maxChars = static_cast<std::size_t>(luaStateWrapper.getTableNumber(tableName, Keys::MAX_CHARS, Constants::DEFAULT_MAX_CHARS));

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

  void InputComponent::applyStyle() {
    std::istringstream classes(getClass());
    std::string cls;
    while (classes >> cls) {
      std::string selector = "." + cls;
      Project::Services::Style s = StyleManager::getInstance().getStyle(selector);
      if (s.width > 0) rect.w = s.width;
      if (s.height > 0) rect.h = s.height;
      if (s.background.a != 0) bgColor = s.background;
      if (s.borderColor.a != 0) borderColor = s.borderColor;
      if (s.borderWidth > 0) borderWidth = s.borderWidth;
      if (s.fontColor.a != 0) textColor = s.fontColor;
      if (s.fontSize > 0 && fontPath.size() > 0) {
        if (font) TTF_CloseFont(font);
        fontSize = s.fontSize;
        font = TTF_OpenFont(fontPath.c_str(), fontSize);
        if (!font) {
          logsManager.logWarning(std::string("Failed to load font: ") + fontPath + ". Using fallback font.");
          font = TTF_OpenFont(Project::Libraries::Constants::DEFAULT_FONT_PATH, fontSize);
        }
      }
    }
    createTexture();
  }

  void InputComponent::setEntityPosition(int x, int y) {
    rect.x = x;
    rect.y = y;
  }

  void InputComponent::setText(const std::string& text) {
    currentText = text;
    createTexture();
  }

  void InputComponent::createTexture() {
    destroyTexture();
    textureW = 0;
    textureH = 0;
    if (!font || currentText.empty()) return;

    SDL_Surface* surface = TTF_RenderText_Blended(font, currentText.c_str(), textColor);
    if (!surface) {
      logsManager.logWarning(std::string("Failed to render input text: ") + TTF_GetError());
      return;
    }
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    textureW = surface->w;
    textureH = surface->h;
    rect.w = rect.w ? rect.w : surface->w + Constants::INDEX_EIGHT;
    rect.h = rect.h ? rect.h : surface->h + Constants::INDEX_EIGHT;
    SDL_FreeSurface(surface);
  }

  void InputComponent::processInput(const Uint8* state, float deltaTime) {
   bool shift = state[SDL_SCANCODE_LSHIFT] || state[SDL_SCANCODE_RSHIFT];
    bool textChanged = false;

    auto wasPressed = [&](SDL_Scancode sc){ return state[sc] && !prevKeys[sc]; };

    bool bsDown = state[SDL_SCANCODE_BACKSPACE];
    if (wasPressed(SDL_SCANCODE_BACKSPACE)) {
      if (caretPos > 0 && !currentText.empty()) {
        currentText.erase(caretPos-1, 1);
        --caretPos;
        textChanged = true;
      }
      backspaceHeld = true;
      backspaceTimer = 0.4f;
    } else if (bsDown && backspaceHeld) {
      backspaceTimer -= deltaTime;
      if (backspaceTimer <= 0.0f) {
        if (caretPos > 0 && !currentText.empty()) {
          currentText.erase(caretPos-1, 1);
          --caretPos;
          textChanged = true;
        }
        backspaceTimer = Constants::DEFAULT_HALF;
      }
    }
    if (!bsDown) {
      backspaceHeld = false;
      backspaceTimer = 0.0f;
    }
    if (wasPressed(SDL_SCANCODE_SPACE) && currentText.size() < maxChars) {
      currentText.insert(caretPos, 1, ' ');
      ++caretPos;
      textChanged = true;
    }
    for (int i = SDL_SCANCODE_A; i <= SDL_SCANCODE_Z; ++i) {
      if (wasPressed(static_cast<SDL_Scancode>(i)) && currentText.size() < maxChars) {
        char c = static_cast<char>('a' + (i - SDL_SCANCODE_A));
        if (shift) c = static_cast<char>(toupper(c));
        currentText.insert(caretPos, 1, c);
        ++caretPos;
        textChanged = true;
      }
    }
    for (int i = SDL_SCANCODE_0; i <= SDL_SCANCODE_9; ++i) {
      if (wasPressed(static_cast<SDL_Scancode>(i)) && currentText.size() < maxChars) {
        char c = static_cast<char>('0' + (i - SDL_SCANCODE_0));
        currentText.insert(caretPos, 1, c);
        ++caretPos;
        textChanged = true;
      }
    }
    if (wasPressed(SDL_SCANCODE_LEFT)) {
      if (caretPos > 0) {
        --caretPos;
      }
    }
    if (wasPressed(SDL_SCANCODE_RIGHT)) {
      if (caretPos < currentText.size()) {
        ++caretPos;
      }
    }
    if (textChanged) createTexture();
  }
}
