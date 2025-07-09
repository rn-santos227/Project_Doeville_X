#include "ComponentsFactory.h"
#include "libraries/constants/Constants.h"
#include "libraries/keys/Keys.h"
#include "utilities/color/ColorUtils.h"

namespace Project::Factories {
  using Project::Components::BaseComponent;
  using Project::Components::BoundingBoxComponent;
  using Project::Components::GraphicsComponent;
  using Project::Components::TextComponent;
  using Project::Handlers::ResourcesHandler;
  using Project::Utilities::ColorUtils;
  using Project::Utilities::ConfigReader;
  using Project::Utilities::LogsManager;
  using Project::Utilities::LuaStateWrapper;

  namespace Constants = Project::Libraries::Constants;
  namespace Keys = Project::Libraries::Keys;

  ComponentsFactory::ComponentsFactory(ConfigReader& configReader, LogsManager& logsManager, ResourcesHandler& resourcesHandler)
  : configReader(configReader), logsManager(logsManager), resourcesHandler(resourcesHandler), renderer(nullptr), keyHandler(nullptr) {}

  std::unique_ptr<BaseComponent> ComponentsFactory::create(const std::string& componentName, LuaStateWrapper& luaStateWrapper, const std::string& tableName) {
    if(logsManager.checkAndLogError(!renderer, "Renderer is null for component: " + componentName)) {
      return nullptr;
    }

    std::string resolvedName = luaStateWrapper.getTableString(tableName, Keys::COMPONENT, componentName);
    ComponentType type = ComponentTypeResolver::resolve(resolvedName);
    
    switch (type) {
      case ComponentType::BOUNDING_BOX:
        return createBoundingBoxComponent(luaStateWrapper, tableName);

      case ComponentType::GRAPHICS:
        return createGraphicsComponent(luaStateWrapper, tableName);

      case ComponentType::TEXT:
        return createTextComponent(luaStateWrapper, tableName);

      default:
        logsManager.logError("Unknown or unsupported component type: " + componentName);
        return nullptr;
    }
  }

  // Components Builder Section
 std::unique_ptr<BaseComponent> ComponentsFactory::createBoundingBoxComponent(LuaStateWrapper& luaStateWrapper, const std::string& tableName) {
    SDL_Color defaultColor = Constants::DEFAULT_DEBUG_TEXT_COLOR;
    SDL_Color debugColor = configReader.getColorValue(Keys::FONT_SECTION, Keys::FONT_DEFAULT_COLOR, defaultColor);
    auto boxComponent = std::make_unique<BoundingBoxComponent>(logsManager, renderer, keyHandler, debugColor);

    lua_State* L = luaStateWrapper.get();
    lua_getglobal(L, tableName.c_str());
    if (lua_istable(L, -1)) {
      lua_getfield(L, -1, Keys::BOXES);
      if (lua_istable(L, -1)) {
        lua_pushnil(L);
        while (lua_next(L, -2)) {
          if (lua_istable(L, -1)) {
            int x = 0, y = 0, w = 0, h = 0;
            lua_getfield(L, -1, Keys::X);
            if (lua_isnumber(L, -1)) x = static_cast<int>(lua_tonumber(L, -1));
            lua_pop(L, 1);
            lua_getfield(L, -1, Keys::Y);
            if (lua_isnumber(L, -1)) y = static_cast<int>(lua_tonumber(L, -1));
            lua_pop(L, 1);
            lua_getfield(L, -1, Keys::W);
            if (lua_isnumber(L, -1)) w = static_cast<int>(lua_tonumber(L, -1));
            lua_pop(L, 1);
            lua_getfield(L, -1, Keys::H);
            if (lua_isnumber(L, -1)) h = static_cast<int>(lua_tonumber(L, -1));
            lua_pop(L, 1);
            SDL_Rect rect{x, y, w, h};
            boxComponent->addBox(rect);
          }
          lua_pop(L, 1);
        }
        lua_pop(L, 1);
      }
      lua_pop(L, 1);
    } else {
      lua_pop(L, 1);
    }

    bool active = luaStateWrapper.getTableBoolean(tableName, Keys::ACTIVE, true);
    boxComponent->setActive(active);

    bool solid = luaStateWrapper.getTableBoolean(tableName, Keys::SOLID, false);
    boxComponent->setSolid(solid);

    return boxComponent;
 }

  std::unique_ptr<BaseComponent> ComponentsFactory::createKeysComponent(LuaStateWrapper& luaStateWrapper, const std::string& tableName) {
    auto keysComponent = std::make_unique<Components::KeysComponent>(logsManager, keyHandler);
  }

  std::unique_ptr<BaseComponent> ComponentsFactory::createGraphicsComponent(LuaStateWrapper& luaStateWrapper, const std::string& tableName) {
    std::string imagePath = luaStateWrapper.getTableString(tableName, Keys::TEXTURE_PATH, "");

    auto graphicsComponent = std::make_unique<GraphicsComponent>(renderer, logsManager);

    if (!imagePath.empty()) {
      graphicsComponent->setTexture(resourcesHandler, imagePath);
    } else {
      int width = static_cast<int>(luaStateWrapper.getTableNumber(tableName, Keys::WIDTH, Constants::DEFAULT_COMPONENT_SIZE));
      int height = static_cast<int>(luaStateWrapper.getTableNumber(tableName, Keys::HEIGHT, Constants::DEFAULT_COMPONENT_SIZE));
      std::string colorHex = luaStateWrapper.getTableString(tableName, Keys::COLOR_HEX, Constants::DEFAULT_SHAPE_COLOR_HEX);
      Uint8 alpha = static_cast<Uint8>(luaStateWrapper.getTableNumber(tableName, Keys::COLOR_ALPHA, Constants::FULL_ALPHA));
      SDL_Color color = ColorUtils::hexToRGB(colorHex, alpha);
      graphicsComponent->setShape(width, height, color);
    }

    bool active = luaStateWrapper.getTableBoolean(tableName, Keys::ACTIVE, true);
    graphicsComponent->setActive(active);

    graphicsComponent->onAttach();
    return graphicsComponent;
  }

  std::unique_ptr<BaseComponent> ComponentsFactory::createMotionComponent(LuaStateWrapper& luaStateWrapper, const std::string& tableName) {
    float speed = static_cast<float>(luaStateWrapper.getTableNumber(tableName, Keys::SPEED, Constants::DEFAULT_MOTION_SPEED));
    auto motionComponent = std::make_unique<Components::MotionComponent>(logsManager, keyHandler, speed);

    bool active = luaStateWrapper.getTableBoolean(tableName, Keys::ACTIVE, true);
    motionComponent->setActive(active);
    return motionComponent;
  }

  std::unique_ptr<BaseComponent> ComponentsFactory::createTextComponent(LuaStateWrapper& luaStateWrapper, const std::string& tableName) {
    std::string colorHex = luaStateWrapper.getTableString(tableName, Keys::FONT_COLOR_HEX, Constants::DEFAULT_SHAPE_COLOR_HEX);
    Uint8 alpha = static_cast<Uint8>(luaStateWrapper.getTableNumber(tableName, Keys::FONT_COLOR_ALPHA, Constants::FULL_ALPHA));
    SDL_Color color = ColorUtils::hexToRGB(colorHex, alpha);

    std::string text = luaStateWrapper.getTableString(tableName, Keys::TEXT, Keys::DEFAULT_TEXT);

    std::string defaultFontPath = configReader.getValue(Keys::FONT_SECTION, Keys::FONT_DEFAULT_PATH, Keys::DEFAULT_FONT_PATH);
    std::string fontPath = luaStateWrapper.getTableString(tableName, Keys::FONT_PATH, defaultFontPath);

    int defaultFontSize = configReader.getIntValue(Keys::FONT_SECTION, Keys::FONT_DEFAULT_SIZE, Constants::DEFAULT_FONT_SIZE);
    int fontSize = static_cast<int>(luaStateWrapper.getTableNumber(tableName, Keys::FONT_SIZE, static_cast<float>(defaultFontSize)));

    TTF_Font* font = TTF_OpenFont(fontPath.c_str(), fontSize);
    if (logsManager.checkAndLogError(font == nullptr, "Failed to load font: " + fontPath)) {
      return nullptr;
    }

    auto textComponent = std::make_unique<TextComponent>(renderer, font, color, text, fontPath, fontSize, logsManager);
    bool active = luaStateWrapper.getTableBoolean(tableName, Keys::ACTIVE, true);
    textComponent->setActive(active);
    return textComponent;
  }

  // Utilities Section
  Uint8 ComponentsFactory::getLuaColorChannel(LuaStateWrapper& luaStateWrapper, const std::string& globalName, Uint8 defaultValue) {
    return static_cast<Uint8>(luaStateWrapper.getGlobalNumber(globalName, static_cast<float>(defaultValue)));
  }

  SDL_Color ComponentsFactory::getLuaSDLColor(LuaStateWrapper& luaStateWrapper) {
    std::string hex = luaStateWrapper.getGlobalString(Keys::FONT_COLOR_HEX, Constants::DEFAULT_SHAPE_COLOR_HEX);
    Uint8 alpha = getLuaColorChannel(luaStateWrapper, Keys::FONT_COLOR_ALPHA);
    return ColorUtils::hexToRGB(hex, alpha);
  }

  std::string ComponentsFactory::getLuaGlobalString(LuaStateWrapper& lua, const std::string& name, const std::string& defaultValue) {
    return lua.getGlobalString(name, defaultValue);
  }

  int ComponentsFactory::getLuaGlobalInt(LuaStateWrapper& lua, const std::string& name, int defaultValue) {
    return static_cast<int>(lua.getGlobalNumber(name, static_cast<float>(defaultValue)));
  }


  // Getters and Setters Section
  void ComponentsFactory::setRenderer(SDL_Renderer* renderer) {
    this->renderer = renderer;
  }

  void ComponentsFactory::setKeyHandler(Handlers::KeyHandler* keyHandler) {
    this->keyHandler = keyHandler;
  }
}
